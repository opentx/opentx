file(GLOB translations *.h.txt)
foreach(translation ${translations})
  get_filename_component(language ${translation} NAME_WE)
  get_filename_component(path ${translation} DIRECTORY)
  set(target ${language}.h)
  if(NOT(PCB STREQUAL X12S OR PCB STREQUAL X10) AND language STREQUAL "cz")
    set(language cz_reduced)
  endif()
  if(PYTHONINTERP_FOUND)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${RADIO_DIRECTORY}/util/translate.py ${translation} ${target} ${language}
      DEPENDS ${translation}
      )
  endif()
  list(APPEND translations_files ${target})
endforeach()

add_custom_target(firmware_translations DEPENDS ${translations_files})
