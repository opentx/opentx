file(GLOB translations *.h.txt)
foreach(translation ${translations})
  get_filename_component(language ${translation} NAME_WE)
  get_filename_component(path ${translation} DIRECTORY)
  set(target ${language}.h)
  add_custom_command(
    OUTPUT ${target}
    COMMAND python ${RADIO_DIRECTORY}/util/translate.py ${translation} ${target} ${language}
    DEPENDS ${translation}
  )
  list(APPEND translations_files ${target})
endforeach()

add_custom_target(firmware_translations DEPENDS ${translations_files})