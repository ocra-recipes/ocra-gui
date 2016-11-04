#
# WATCH OUT!!!! Don't use glob to get the source files here... It picks up a weird cmake binary and throws linker errors.
#



cmake_minimum_required(VERSION 2.8.11)

project(ocra-gui)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(YARP REQUIRED)
#find_package(gazebo REQUIRED)
find_package(OcraIcub REQUIRED)
IF(${OcraIcub_FOUND})
    message("-- Found OcraIcub version ${OcraIcub_VERSION}")
ENDIF()

include_directories(
#    ${GAZEBO_INCLUDE_DIRS}
    ${YARP_INCLUDE_DIRS}
    ${OcraIcub_INCLUDE_DIRS}
                    )

link_directories(
#                ${GAZEBO_LIBRARY_DIRS}
                ${YARP_LIBRARY_DIRS}
                )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GAZEBO_CXX_FLAGS}")


set(Sources
    main.cpp
    mainwindow.cpp
    taskwidget.cpp
    gazebotools.cpp
    physicstools.cpp
    yarptools.cpp
    connectionmonitor.cpp
    )

set(Headers
    mainwindow.h
    taskwidget.h
    gazebotools.h
    physicstools.h
    yarptools.h
    connectionmonitor.h
    )

set(Forms
    mainwindow.ui
    taskwidget.ui
    )

 qt5_add_resources(Resources
    resources.qrc
)



if(${CMAKE_VERSION} VERSION_LESS 3.0.2)
	message("-- Using cmake < 3.0.2, must wrap ui using QT5_WRAP_UI")
        QT5_WRAP_UI(UI_FORMS ${Forms})
endif()


add_executable(${PROJECT_NAME}
    ${Sources}
    ${Headers}
    ${Forms}
    ${Resources}
    )

target_link_libraries(${PROJECT_NAME}
Qt5::Core
Qt5::Gui
Qt5::Widgets
${YARP_LIBRARIES}
#${GAZEBO_LIBRARIES}
#${Boost_LIBRARIES}
${OcraIcub_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}  DESTINATION bin)
