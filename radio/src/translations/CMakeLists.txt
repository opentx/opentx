file(GLOB translations *.h.txt)

foreach(translation ${translations})
  get_filename_component(language ${translation} NAME_WE)
  get_filename_component(path ${translation} DIRECTORY)
  set(target ${language}.h)
  if(PCB STREQUAL X12S OR PCB STREQUAL X10 OR PCB STREQUAL NV14)
    set(language all)
  endif()
  if(PYTHONINTERP_FOUND)
    add_custom_command(
      OUTPUT ${target}
      COMMAND ${PYTHON_EXECUTABLE} ${TOOLS_DIRECTORY}/encode-translation.py ${translation} ${target} ${language}
      DEPENDS ${translation} ${TOOLS_DIRECTORY}/encode-translation.py ${TOOLS_DIRECTORY}/charset.py
      )
  endif()
  list(APPEND translations_files ${target})
endforeach()

add_custom_target(firmware_translations DEPENDS ${translations_files})
