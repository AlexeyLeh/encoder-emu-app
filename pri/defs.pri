# defs.pri

isEmpty( _NL_ ) : _NL_ = $$escape_expand( \\n\\t )
isEmpty( QTBIN ): QTBIN = $$dirname(QMAKE_QMAKE)

isEmpty( _SL_ ) {
   unix : _SL_ = /
   win32: _SL_ = "\\"
}

#DEST_LOCALE = ..$${_SL_}..$${_SL_}mess

#------------------------------------------------
unix {
   DESTDIR  = .
} else:mingw {
   #DESTDIR = release
   #DESTDIR = debug
   #CONFIG(debug, debug|release) {
   CONFIG(debug, debug|release) {
      DESTDIR  = debug
   } else {
      DESTDIR  = release
   }
} else:win32 {
   CONFIG(debug, debug|release) {
      DESTDIR  = debug
   } else {
      DESTDIR  = release
   }
}

#------------------------------------------------
mingw|unix {
   QMAKE_CXXFLAGS += -Wno-write-strings

} else:win32 {
   QMAKE_CFLAGS_WARN_ON -= -W3 -W4
   QMAKE_CFLAGS_WARN_ON += -W4

   QMAKE_CXXFLAGS_WARN_ON -= -W3 -W4
   QMAKE_CXXFLAGS_WARN_ON += -W4

   contains( CONFIG, all_warnings ) {
      QMAKE_CXXFLAGS_WARN_ON -= -wd4505 -w34100 -w34189 -wd4100
      QMAKE_CXXFLAGS_WARN_ON -= -wd4127 -wd4239 -wd4512 -wd4481 -wd4718
   } else {
      #'function' : unreferenced local function has been removed
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4505 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4505

      #The formal parameter is not referenced in the body of the function
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4100 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4100

      #conditional expression is constant
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4127 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4127

      #nonstandard extension used : 'token' : conversion from 'type' to 'type'
      #!contains( QMAKE_CXXFLAGS_WARN_ON, -wd4239 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4239

      #'class' : assignment operator could not be generated
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4512 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4512

      #nonstandard extension used: override specifier 'override'
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4481 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4481

      #recursive call has no side effects, deleting
      !contains( QMAKE_CXXFLAGS_WARN_ON, -wd4718 ) : QMAKE_CXXFLAGS_WARN_ON += -wd4718
   }
}

#-------------------------------------------------
win32 {
   QMAKE_DEL_FILE = del /Q /F
   QMAKE_DEL_DIR  = rd /Q /S

   # command utils:
   MV    = move /y
   CP    = copy /y
   CD    = xcopy /s /q /y /i
   RM    = del /q /f
   RD    = rd  /q /s
   MD    = md

   _SEP_ = "&& "
   _1NUL_ = " 1> nul"
   _2NUL_ = " 2> nul"
   _EXE_  =".exe"
}

#-------------------------------------------------
unix {
   # command utils:
   MV = mv -f
   CP = cp -f
   CD = cp -fr
   RM = rm -f
   RD = rm -fr
   MD = mkdir -p -m 770

   _SEP_ = "&& "
   _2NUL_ = " 2>/dev/null"
}


#-------------------------------------------------
Debug|Release|unix {
   QMAKE_CLEAN += *~ *.suo *.pdb
   QMAKE_CLEAN += release\\*.pdb debug\\*.pdb
   QMAKE_CLEAN += release\\*.exe debug\\*.exe
   QMAKE_CLEAN += release\\*.Release debug\\*.Debug
   #QMAKE_CLEAN += *.$${TARGET}.Debug *.$${TARGET}.Release

   #QMAKE_CLEAN += $${TARGET}$${_EXE_}
   #QMAKE_CLEAN += release\\${TARGET}$${_EXE_}
   #QMAKE_CLEAN += debug\\$${TARGET}$${_EXE_}
   ##QMAKE_CLEAN += ipch\\*.*

   #QMAKE_DISTCLEAN += release\\*.*
   #QMAKE_DISTCLEAN += debug\\*.*
   #QMAKE_DISTCLEAN += ipch\\*.*
   #QMAKE_DISTCLEAN += *.pdb
   #QMAKE_DISTCLEAN += *.sdf
   #QMAKE_DISTCLEAN += 2> nul
   #QMAKE_DISTCLEAN += & rd debug release obj ipch dist
   #QMAKE_DISTCLEAN += 2> nul

   QMAKE_DISTCLEAN += debug\\ release\\ obj\\ ipch\\ dist\\

   #distclean.depends += ext_clean
   #ext_clean.commands = $${QMAKE_DEL_DIR} debug release obj ipch
   #QMAKE_EXTRA_TARGETS += ext_clean distclean
}
