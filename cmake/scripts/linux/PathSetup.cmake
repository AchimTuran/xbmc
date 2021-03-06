include(GNUInstallDirs)

if(NOT prefix)
  set(prefix ${CMAKE_INSTALL_PREFIX})
else()
  set(CMAKE_INSTALL_PREFIX ${prefix})
endif()
if(NOT exec_prefix)
  set(exec_prefix ${prefix})
endif()
if(NOT libdir)
  set(libdir ${CMAKE_INSTALL_FULL_LIBDIR})
endif()
if(NOT bindir)
  set(bindir ${CMAKE_INSTALL_FULL_BINDIR})
endif()
if(NOT includedir)
  set(includedir ${CMAKE_INSTALL_FULL_INCLUDEDIR})
endif()
if(NOT datarootdir)
  set(datarootdir ${CMAKE_INSTALL_FULL_DATAROOTDIR})
endif()
if(NOT datadir)
  set(datadir ${CMAKE_INSTALL_FULL_DATADIR})
endif()
if(NOT docdir)
  set(docdir ${CMAKE_INSTALL_FULL_DOCDIR})
endif()

list(APPEND final_message "-- PATH config --")
list(APPEND final_message "Prefix: ${prefix}")
list(APPEND final_message "Libdir: ${libdir}")
list(APPEND final_message "Bindir: ${bindir}")
list(APPEND final_message "Includedir: ${includedir}")
list(APPEND final_message "Datarootdir: ${datarootdir}")
list(APPEND final_message "Datadir: ${datadir}")
list(APPEND final_message "Docdir: ${docdir}")

set(PATH_DEFINES -DBIN_INSTALL_PATH=\"${libdir}/${APP_NAME_LC}\"
                   -DINSTALL_PATH=\"${datarootdir}/${APP_NAME_LC}\")
