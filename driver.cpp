#define ASMJIT_STATIC
#include <asmjit/asmjit.h>
#include <stdio.h>


using namespace asmjit;

// Signature of the generated function.
typedef int (*Func)(void);

int basic()
{
  JitRuntime rt;                          // Runtime designed for JIT code execution.

  CodeHolder code;                        // Holds code and relocation information.
  code.init(rt.environment());            // Initialize CodeHolder to match JIT environment.

  x86::Assembler a(&code);                // Create and attach x86::Assembler to `code`.
  a.mov(x86::rax, 1);                     // Move one to 'eax' register.
  a.ret();                                // Return from function.
  // ----> x86::Assembler is no longer needed from here and can be destroyed <----

  Func fn;
  Error err = rt.add(&fn, &code);         // Add the generated code to the runtime.
  if (err) return 1;                      // Handle a possible error returned by AsmJit.
  // ----> CodeHolder is no longer needed from here and can be destroyed <----

  int result = fn();                      // Execute the generated code.
  printf("%d\n", result);                 // Print the resulting "1".

  // All classes use RAII, all resources will be released before `main()` returns,
  // the generated function can be, however, released explicitly if you intend to
  // reuse or keep the runtime alive, which you should in a production-ready code.
  rt.release(fn);

  return 0;
}

int testFuncSignature()
{
  JitRuntime rt;                          

  CodeHolder code;                       
  code.init(rt.environment());            

  x86::Compiler cc(&code);                  
  FuncSignatureBuilder signature(CallConv::kIdHost);
  signature.setRetT<int>();
  //  for (i = 0; i < argCount; i++)
  //    signature.addArgT<int>();

  cc.addFunc(signature);

  // cc.addFunc(FuncSignatureT<int,void>());      

  x86::Gp vReg = cc.newGpd();              
  cc.mov(vReg, 1);                        
  cc.ret(vReg);                           // Return `vReg` from the function.

  cc.endFunc();                           // End of the function body.
  cc.finalize();                          // Translate and assemble the whole `cc` content.
  // ----> X86Compiler is no longer needed from here and can be destroyed <----

  Func fn;
  Error err = rt.add(&fn, &code);         // Add the generated code to the runtime.
  if (err) return 1;                      // Handle a possible error returned by AsmJit.
  // ----> CodeHolder is no longer needed from here and can be destroyed <----

  int result = fn();                      // Execute the generated code.
  printf("%d\n", result);                 // Print the resulting "1".

  rt.release(fn);                         // RAII, but let's make it explicit.
  return 0;

}

int main(int argc, char* argv[]) {
  basic();
  testFuncSignature();
  return 0;
}
