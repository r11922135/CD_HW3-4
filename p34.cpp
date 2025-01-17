/*
Pin-Chun Huang 0510419,
Yung-Yao Cheng 0510420,
Yunseo Park 0510172
*/

// STL
#include <map>
#include <utility>
#include <vector>
#include <set>

// LLVM
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include <llvm/IR/PassManager.h>
// add additional includes from LLVM or STL as needed
#include "llvm/IR/Instructions.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/IRBuilder.h"


using namespace llvm;

namespace {

class DefinitionPass : public PassInfoMixin<DefinitionPass> {
public:

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    // TODO
    // Map for in[I] and out[I] for each Instruction
    std::map<Instruction*, std::set<Value*>> inSets;
    std::map<Instruction*, std::set<Value*>> outSets;

    // Compute gen[I] and kill[I] for each Instruction
    std::map<Instruction*, std::set<Value*>> genSets;
    std::map<Instruction*, std::set<Value*>> killSets;

    // Set to track used but uninitialized variables
    std::set<Value*> uninitializedVariables;

    // Initialize gen and kill sets for instructions
    for (auto &BB : F) {
      for (auto &I : BB) {
        std::set<Value*> gen;
        std::set<Value*> kill;

        if (auto *AI = dyn_cast<AllocaInst>(&I)) {
          // If variable is declared but not initialized yet
          gen.insert(AI);
        }

        else if (auto *SI = dyn_cast<StoreInst>(&I)) {
          Value *var = SI->getPointerOperand();
          kill.insert(var);
          //gen.erase(var); // Initialization removes from gen
        }

        genSets[&I] = gen;
        killSets[&I] = kill;
      }
    }

    // Initialize in[I] and out[I] for each instruction
    for (auto &BB : F) {
      for (auto &I : BB) {
        inSets[&I] = {};
        outSets[&I] = {};
      }
    }

    // Iteratively compute in[I] and out[I] until convergence
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto &BB : F) {
        for (auto &I : BB) {
          Instruction *inst = &I;
          std::set<Value*> newIn;

          // For the first instruction in a basic block, consider predecessors
          if (inst == &BB.front()) {
            for (auto *predBB : predecessors(&BB)) {
              Instruction *termInst = predBB->getTerminator(); // Last instruction in pred block
              if (termInst) {
                newIn.insert(outSets[termInst].begin(), outSets[termInst].end());
              }
            }
          } else {
            // For other instructions, inherit from the previous instruction
            if (auto *predInst = inst->getPrevNode()) {
              newIn.insert(outSets[predInst].begin(), outSets[predInst].end());
            }
          }

          // Compute out[I]: gen[I] union (in[I] - kill[I])
          std::set<Value*> newOut = genSets[inst];
          std::set<Value*> diff;
          std::set_difference(newIn.begin(), newIn.end(), killSets[inst].begin(), killSets[inst].end(), std::inserter(diff, diff.end()));
          newOut.insert(diff.begin(), diff.end());

          // Check if in or out changed
          if (newIn != inSets[inst] || newOut != outSets[inst]) {
            changed = true;
            inSets[inst] = newIn;
            outSets[inst] = newOut;
          }
        }
      }
    }

    // Check for uninitialized variables being used
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
          Value *var = LI->getPointerOperand();
          if (inSets[&I].count(var)) {
            uninitializedVariables.insert(var);
          }
        }
      }
    }

    for (auto *var : uninitializedVariables) {
      errs() << var->getName() << "\n";
    }

    //errs() << "def-pass\n";
    return PreservedAnalyses::all();
  }

  static bool isRequired() { return true; }
};

class FixingPass : public PassInfoMixin<FixingPass> {
public:

  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
    // TODO
    // Map for in[I] and out[I] for each Instruction
    std::map<Instruction*, std::set<Value*>> inSets;
    std::map<Instruction*, std::set<Value*>> outSets;

    // Compute gen[I] and kill[I] for each Instruction
    std::map<Instruction*, std::set<Value*>> genSets;
    std::map<Instruction*, std::set<Value*>> killSets;

    // Set to track used but uninitialized variables
    std::set<Value*> uninitializedVariables;

    // Initialize gen and kill sets for instructions
    for (auto &BB : F) {
      for (auto &I : BB) {
        std::set<Value*> gen;
        std::set<Value*> kill;

        if (auto *AI = dyn_cast<AllocaInst>(&I)) {
          // If variable is declared but not initialized yet
          gen.insert(AI);
        }

        else if (auto *SI = dyn_cast<StoreInst>(&I)) {
          Value *var = SI->getPointerOperand();
          kill.insert(var);
          //gen.erase(var); // Initialization removes from gen
        }

        genSets[&I] = gen;
        killSets[&I] = kill;
      }
    }

    // Initialize in[I] and out[I] for each instruction
    for (auto &BB : F) {
      for (auto &I : BB) {
        inSets[&I] = {};
        outSets[&I] = {};
      }
    }

    // Iteratively compute in[I] and out[I] until convergence
    bool changed = true;
    while (changed) {
      changed = false;
      for (auto &BB : F) {
        for (auto &I : BB) {
          Instruction *inst = &I;
          std::set<Value*> newIn;

          // For the first instruction in a basic block, consider predecessors
          if (inst == &BB.front()) {
            for (auto *predBB : predecessors(&BB)) {
              Instruction *termInst = predBB->getTerminator(); // Last instruction in pred block
              if (termInst) {
                newIn.insert(outSets[termInst].begin(), outSets[termInst].end());
              }
            }
          } else {
            // For other instructions, inherit from the previous instruction
            if (auto *predInst = inst->getPrevNode()) {
              newIn.insert(outSets[predInst].begin(), outSets[predInst].end());
            }
          }

          // Compute out[I]: gen[I] union (in[I] - kill[I])
          std::set<Value*> newOut = genSets[inst];
          std::set<Value*> diff;
          std::set_difference(newIn.begin(), newIn.end(), killSets[inst].begin(), killSets[inst].end(), std::inserter(diff, diff.end()));
          newOut.insert(diff.begin(), diff.end());

          // Check if in or out changed
          if (newIn != inSets[inst] || newOut != outSets[inst]) {
            changed = true;
            inSets[inst] = newIn;
            outSets[inst] = newOut;
          }
        }
      }
    }

    // Check for uninitialized variables being used
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *LI = dyn_cast<LoadInst>(&I)) {
          Value *var = LI->getPointerOperand();
          if (inSets[&I].count(var)) {
            uninitializedVariables.insert(var);
          }
        }
      }
    }
    // Step 4: Traverse instructions and insert initialization immediately after declaration
    for (auto &BB : F) {
      for (auto &I : BB) {
        if (auto *AI = dyn_cast<AllocaInst>(&I)) {
          Value *var = &I;
          if (uninitializedVariables.count(var)) {
            IRBuilder<> builder(AI->getNextNode()); // Insert after declaration
            Value *initialValue = nullptr;

            if (var->getType()->isPointerTy()) {
              Type *elemType = var->getType()->getPointerElementType();
              if (elemType->isIntegerTy()) {
                initialValue = ConstantInt::get(elemType, 10);
              } else if (elemType->isFloatTy()) {
                initialValue = ConstantFP::get(elemType, 20.0);
              } else if (elemType->isDoubleTy()) {
                initialValue = ConstantFP::get(elemType, 30.0);
              }
              if (initialValue) {
                builder.CreateStore(initialValue, AI);
              }
            }
          }
        }
      }
    }

    //errs() << "fix-pass\n";
    return PreservedAnalyses::none();
  }

  static bool isRequired() { return true; }
};
} // namespace

// Pass registrations
llvm::PassPluginLibraryInfo getP34PluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "P34", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, llvm::FunctionPassManager &PM,
                   ArrayRef<llvm::PassBuilder::PipelineElement>) {
                  if (Name == "def-pass") {
                    PM.addPass(DefinitionPass());
                    return true;
                  }
                  if (Name == "fix-pass") {
                    PM.addPass(FixingPass());
                    return true;
                  }
                  return false;
                });
          }};
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getP34PluginInfo();
}

