// Copyright (c) 2014 Dropbox, Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//    http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PYSTON_CODEGEN_CODEGEN_H
#define PYSTON_CODEGEN_CODEGEN_H

#include <unordered_map>

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"

#include "core/types.h"

#include "codegen/gcbuilder.h"
#include "codegen/runtime_hooks.h"

namespace pyston {

class PystonJITEventListener;

class FunctionAddressRegistry {
    private:
        struct FuncInfo {
            std::string name;
            int length;
            llvm::Function *llvm_func;
            FuncInfo(const std::string& name, int length, llvm::Function *llvm_func) :
                    name(name), length(length), llvm_func(llvm_func) {
            }
        };
        typedef std::unordered_map<void*, FuncInfo> FuncMap;
        FuncMap functions;
        std::unordered_set<void*> lookup_neg_cache;

    public:
        std::string getFuncNameAtAddress(void* addr, bool demangle, bool *out_success=NULL);
        llvm::Function* getLLVMFuncAtAddress(void* addr);
        void registerFunction(const std::string &name, void *addr, int length, llvm::Function* llvm_func);
        void dumpPerfMap();
};

llvm::JITEventListener* makeRegistryListener();
llvm::JITEventListener* makeTracebacksListener();

struct GlobalState {
    llvm::LLVMContext &context;
    llvm::Module *stdlib_module, *cur_module;
    llvm::TargetMachine *tm;
    llvm::ExecutionEngine *engine;

    std::vector<llvm::JITEventListener*> jit_listeners;

    FunctionAddressRegistry func_addr_registry;
    llvm::Type *llvm_value_type, *llvm_value_type_ptr;
    llvm::Type *llvm_class_type, *llvm_class_type_ptr;
    llvm::Type *llvm_flavor_type, *llvm_flavor_type_ptr;
    llvm::Type *llvm_opaque_type;
    llvm::Type *llvm_str_type_ptr;
    llvm::Type *llvm_clfunction_type_ptr;
    llvm::Type *llvm_module_type_ptr, *llvm_bool_type_ptr;
    llvm::Type *i1, *i8, *i8_ptr, *i32, *i64, *void_, *double_;

    GlobalFuncs funcs;

    GlobalState() : context(llvm::getGlobalContext()) {
    };
};

extern GlobalState g;

// in runtime_hooks.cpp:
void initGlobalFuncs(GlobalState &g);

}

#endif