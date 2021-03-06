#################
# LinkOVR.cmake #
#################

IF(WITH_OVR)
  TARGET_LINK_LIBRARIES(${targetname} ${OVR_LIBRARY})

  IF(APPLE)
    TARGET_LINK_LIBRARIES(${targetname} ${COCOA} ${COREFOUNDATION} ${COREGRAPHICS} ${IOKIT})
  ENDIF()

  IF("${CMAKE_SYSTEM}" MATCHES "Linux")
    TARGET_LINK_LIBRARIES(${targetname} dl pthread rt ${X11_X11_LIB} ${X11_Xrandr_LIB} ${X11_Xxf86vm_LIB})
  ENDIF()

  IF(MSVC_IDE)
    TARGET_LINK_LIBRARIES(${targetname} winmm ws2_32)

    IF(MSVC_VERSION GREATER_EQUAL 1900)
      TARGET_LINK_LIBRARIES(${targetname} legacy_stdio_definitions)
    ENDIF()
  ENDIF()
ENDIF()
