# Module handling macros

# Define a new module
# mod_name -> Target name for this module
# mod_src  -> Source file for this module
# mod_dest -> Destination directory
MACRO(CREATE_MODULE mod_name mod_src mod_dest)
  ADD_LIBRARY(${mod_name} MODULE ${mod_src})
  SET_TARGET_PROPERTIES(${mod_name} PROPERTIES PREFIX "" SUFFIX ${SHLIBEXT})
  IF(APPLE)
    TARGET_LINK_LIBRARIES(${mod_name} ratboxcore ratbox)
  ENDIF(APPLE)
  INSTALL(TARGETS ${mod_name} DESTINATION ${mod_dest})
ENDMACRO(CREATE_MODULE mod_name mod_src mod_dest)
