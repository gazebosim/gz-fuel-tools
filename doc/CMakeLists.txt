find_package(Doxygen)

if (DOXYGEN_FOUND)
  configure_file(${CMAKE_SOURCE_DIR}/doc/gz.in
  ${CMAKE_BINARY_DIR}/gz.dox @ONLY)

  configure_file(${CMAKE_SOURCE_DIR}/doc/tutorials.in
                 ${CMAKE_BINARY_DIR}/tutorials.dox @ONLY)

  add_custom_target(doc ALL
    # Generate the API documentation
    ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/gz.dox
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/tutorials.dox

    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  install(FILES ${CMAKE_BINARY_DIR}/doc/${PROJECT_NAME_LOWER}.tag.xml
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/gz/${PROJECT_NAME_LOWER}_${PROJECT_VERSION_MINOR})
endif()
