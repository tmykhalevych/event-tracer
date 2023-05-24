add_custom_target(examples)

function(add_example EXAMPLE SOURCES)
  add_executable(${EXAMPLE} ${SOURCES})
  add_dependencies(examples ${EXAMPLE})
endfunction()
