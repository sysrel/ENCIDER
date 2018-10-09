file(REMOVE_RECURSE
  "CXX/Output"
  "Concrete/Output"
  "Coverage/Output"
  "Dogfood/Output"
  "Expr/Output"
  "Expr/Lexer/Output"
  "Expr/Parser/Output"
  "Feature/Output"
  "Intrinsics/Output"
  "Merging/Output"
  "Programs/Output"
  "Replay/Output"
  "Replay/libkleeruntest/Output"
  "Runtime/Output"
  "Runtime/POSIX/Output"
  "Runtime/POSIX/SELinux/Output"
  "Runtime/Uclibc/Output"
  "Solver/Output"
  "VectorInstructions/Output"
  "regression/Output"
  "CMakeFiles/systemtests"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/systemtests.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
