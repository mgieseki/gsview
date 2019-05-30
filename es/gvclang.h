/* Copyright (C) 1993-2012, Ghostgum Software Pty Ltd. All rights reserved.

  This file is part of GSview.

  This program is distributed with NO WARRANTY OF ANY KIND.  No author
  or distributor accepts any responsibility for the consequences of using it,
  or for whether it serves any particular purpose or works at all, unless he
  or she says so in writing.  Refer to the GSview Free Public Licence
  (the "Licence") for full details.

  Every copy of GSview must include a copy of the Licence, normally in a
  plain ASCII text file named LICENCE.  The Licence grants you the right
  to copy, modify and redistribute GSview, but only under certain conditions
  described in the Licence.  Among other things, the Licence requires that
  the copyright notice and this notice be preserved on all copies.
*/

/* es\gvclang.h */
/* Common Spanish language defines */

#define AAESPANOL "Espa�ol" /* Espaniol in code page 1252 */

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Reservados todos los derechos."
#define GSVIEW_COPYRIGHT3 "L�ase el archivo LICENCE para m�s detalles."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Reservados todos los derechos."
#define GS_COPYRIGHT3 "L�ase el archivo PUBLIC para m�s detalles."

/* Buttons */
#ifdef UNIX
#define AAOK "Aceptar"
#define AACANCEL "Cancelar"
#define AAEDIT "Editar"
#define AADEFAULTS "Por defecto"
#define AAHELP "Ayuda"
#define AANEW "Nuevo"
#define AATEST "Test"
#define AAYES "Si"
#define AANO "No"
#define AANEXTGT "Siguiente >"
#define AABACKLT "< Anterior"
#define AAFINISHGT "Finalizar >"
#define AAEXIT "Salir"
#else
#ifdef _Windows
#define AAOK "&Aceptar"
#define AACANCEL "&Cancelar"
#define AAEDIT "&Editar"
#define AADEFAULTS "Por defecto"
#define AAHELP "A&yuda"
#define AANEW "N&uevo"
#define AATEST "&Test"
#define AAYES "&Si"
#define AANO "&No"
#define AANEXTGT "S&iguiente >"
#define AABACKLT "< Ante&rior"
#define AAFINISHGT "&Finalizar >"
#define AAEXIT "Sa&lir"
#else
#define AAOK "~Aceptar"
#define AACANCEL "~Cancelar"
#define AAEDIT "~Editar"
#define AADEFAULTS "Por defecto"
#define AAHELP "A~yuda"
#define AANEW "N~uevo"
#define AATEST "~Test"
#define AAYES "~Si"
#define AANO "~No"
#define AANEXTGT "S~iguiente >"
#define AABACKLT "< Ante~rior"
#define AAFINISHGT "~Finalizar >"
#define AAEXIT "Sa~lir"
#endif
#endif

/* About dialog box */
#define AAABOUTWIN "Acerca de GSview para Windows"
#define AAABOUTPM "Acerca de PM GSview"
#define AAABOUTX11 "Acerca de X11 GSview"
#define AAABOUTGSV16SPL "Acerca de GSV16SPL"
#define AAABOUTGSVIEWPRINT "Acerca de GSview Print"
#define AACOPY1 "Versi�n:"
#define AACOPY2 "Un interface gr�fico para Ghostscript"
#define AACOPY4 "Este programa se distribuye SIN NING�N TIPO DE GARANT�A."
#define AACOPY5 "Ni el autor ni los distribuidores aceptan ninguna responsabilidad por las"
#define AACOPY6 "consecuencias de su uso, o por si es �til para cualquier prop�sito"
#define AACOPY7 "o trabajo particular, salvo que �l o ellos la acepten por escrito. Lea"
#define AACOPY8 "la GSview Free Public Licence (la 'Licencia') para m�s informaci�n."
#define AACOPY9 "Cada copia de GSview debe incluir una copia de la Licencia, distribuida"
#define AACOPY10 "normalmente en el archivo de texto ASCII llamado LICENCE.  La Licencia"
#define AACOPY11 "le permite copiar, modificar y redistribuir GSview, pero s�lo bajo"
#define AACOPY12 "ciertas condiciones descritas en la Licencia. Entre otras,"
#define AACOPY13 "la Licencia requiere que la informaci�n acerca del copyright y esta nota"
#define AACOPY14 "aparezcan en todas las copias."
#define AACOPY15 "pstotxt3.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "y tiene una licencia particular.  Lea el archivo pstotext.txt para los detalles."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Un interface en forma DLL para Ghostscript."
#define AACOPY21 "Este programa forma parte de GSview."
#define AACOPY22 "Gestor de impresi�n para GSview Win32s/Win16"

#define AAINPUT "Entrada"

/* DSC error dialog box */
#define AADSC "Reglas de Estructuraci�n de Documentos (DSC)"
#define AAIGNOREALLDSC "Ignorar comentarios DSC"

/* Info dialog box */
#define AAINFO  "Informaci�n"
#define AAFILEC "Archivo:"
#define AATYPEC "Tipo:"
#define AATITLEC "T�tulo:"
#define AADATEC "Fecha:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orientaci�n:"
#define AADEFAULTMEDIAC "Tama�o del papel:"
#define AAPAGEORDERC "Paginaci�n:"
#define AAPAGESC "N� de p�ginas:"
#define AAPAGEC "P�gina:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Sonidos"
#define AAEVENTC "Suceso:"
#define AASOUNDC "Sonido:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Selecci�n del puerto de la impresora"
#define AASELECTPRINTER "Selecci�n de la impresora"
#define AAPRINTING "Imprimiendo"
#define AASELECTPAGE "Selecci�n de la p�gina"
#define AASELECTPAGES "Selecci�n de las p�ginas"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Configuraci�n de la impresora"
#define AADEVICEC "Modelo:"
#define AARESOLUTIONC "Resoluci�n:"
#define AAQUEUEC "Impresora:"
#define AAPRINTTOFILE "Imprimir en un archivo"
#define AAPSPRINTER "Impresora PostScript"
#define AAOPTIONSC "Opciones:"
#define AAFIXEDMEDIA "Papel prefijado"
#define AACONVERT "Convertir"
#define AAWINPRINTERSETTINGS "Configurar la impresora de Windows"
#define AACOLOURS "Colores"
#define AABANDW "Blanco y negro"
#define AAGREY "Tonos grises"
#define AAFULLCOLOUR "A todo color"
#define AASELECTGSDEVICE "Seleccionar dispositivo de salida Ghostscript"

#define AAODDEVEN "Impares/Pares"
#define AAIGNOREDSC "Ignorar comentarios DSC"
#define AAPAGES "P�ginas"
#define AAFROMC "Desde:"
#define AATOC "Hasta:"

#define AAADVANCED "Avanzado"
#define AAADVANCEDPSOPT "Opciones PostScript avanzadas"
#define AASENDCTRLDBEFORE "Enviar CTRL+D antes del trabajo"
#define AASENDCTRLDAFTER "Enviar CTRL+D despu�s del trabajo"
#define AAPROLOGFILE "Archivo pr�logo"
#define AAEPILOGFILE "Archivo ep�logo"
#define AABROWSE "Examinar"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS a EPS"
#define AAPSTOEPSREAD "�Ha le�do la ayuda 'PS a EPS'?"
#define AAPSTOEPSAUTO "C�lculo autom�tico del BoundingBox"

#ifdef UNIX
#define AAAPROPERTIES "Propiedades"
#define AAALLPAGES "Todas las p�ginas"
#define AAODDPAGES "P�ginas impares"
#define AAEVENPAGES "P�ginas pares"
#define AAALL "Todas"
#define AAODD "Impares"
#define AAEVEN "Pares"
#define AAREVERSE "Orden inverso"
#else
#ifdef _Windows
#define AAAPROPERTIES "P&ropiedades"
#define AAALLPAGES "&Todas las p�ginas"
#define AAODDPAGES "P�ginas &impares"
#define AAEVENPAGES "P�ginas &pares"
#define AAALL "&Todas"
#define AAODD "&Impares"
#define AAEVEN "&Pares"
#define AAREVERSE "&Orden inverso"
#else
#define AAAPROPERTIES "P~ropiedades"
#define AAALLPAGES "~Todas las p�ginas"
#define AAODDPAGES "P�ginas ~impares"
#define AAEVENPAGES "P�ginas ~pares"
#define AAALL "~Todas"
#define AAODD "~Impares"
#define AAEVEN "~Pares"
#define AAREVERSE "~Orden inverso"
#endif
#endif

/* Properties dialog box */
#define AAPROPERTIES "Propiedades"
#define AAPROPERTYC "Propiedad:"
#define AAVALUEC "Valor:"
#define AAPAGEOFFSETPT "M�rgenes (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Editar propiedades"
#define AAPROPERTYTYPEC "Tipo de propiedad:"
#define AANUMBER "N�mero"
#define AASTRING "Texto"
#define AANAMEC "Nombre: (por ejemplo,  BitsPerPixel)"
#define AAVALUESC "Valores: (por ejemplo,  1,3,8,16,24)"
#define AADELETE "Eliminar"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Configuraci�n avanzada"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Objeto compartido de Ghostscript:" 
#define AAGHOSTSCRIPTEXEC "Programa Ghostscript:" 
#define AAGHOSTSCRIPTINCC "Camino de b�squeda de Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Opciones de Ghostscript:"
#define AACOPYPRINTERDEF "Copiar los valores predeterminados para la impresora"
#define AAASSOCIATEPS "Asociar los archivos .ps a GSview"
#define AAASSOCIATEPDF "Asociar los archivos .pdf a GSview"
#define AACREATESTARTMENU "Crear las entradas en el men� de Inicio"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Configuraci�n sencilla"
#define AAWHICHGS "�Qu� versi�n de Ghostscript quiere utilizar?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Descargar Ghostscript"
#define AADOWNLOADNOW "Descargar ahora"
#define AAGSNOTFOUND "Ghostscript no se encuentra en su ordenador.  Necesita descargar e instalar AFPL Ghostscript desde"

#define AAGSMESSWIN "GSview para Windows - Mensajes de Ghostscript"
#define AAGSMESSPM "PM GSview - Mensajes de Ghostscript"
#define AAGSMESSX11 "X11 GSview - Mensajes de Ghostscript"
#define AACOPY "Copiar"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Par�metros de pantalla"
#define AARESOLUTION "Resoluci�n"
#define AAZOOMRESOLUTION "Resoluci�n del zoom"
#define AADEPTH "Intensidad"
#define AATEXTALPHA "Definici�n de textos"
#define AAGRAPHICSALPHA "Definici�n de gr�ficos"
#define AADRAWMETHOD "M�todo de dibujo"
#define AADPI "ppp"
#define AABPP "bits/pixel"
#define AABITS "bits"

/* Measure and custom units dialog box */
#define AAPT "pt"
#define AAMM "mm"
#define AAINCH "in"
#define AACUSTOM "Personalizado"
#define AAMEASURE "Medir"
#define AAUNITC "Unidad:"
#define AASTARTC "Inicio:"
#define AAFINISHC "Final:"
#define AADELTAC "Delta:"
#define AALENGTHC "Longitud:"
#define AACHANGE "Cambiar"
#define AACALCXFORM "C�lculo de la transformaci�n"
#define AATRANSLATE "Traslaci�n"
#define AAROTATE "Giro"
#define AASCALE "Escala"
#define AAINITMATRIX "Matriz inicial"
#define AAINVERTMATRIX "Matriz inversa"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS a formato vectorial"
#define AAFORMATC "Formato:"
#define AADRAWTASP "Dibujar el texto usando pol�gonos"
#define AAMAPTOLATIN1 "Llevar a c�digo ISO-Latin1"
#define AAFLATNESSC "Contraste:"
#define AADEFAULTFONTC "Fuente predeterminada:"
#define AADRIVEROPTIONSC "Opciones del controlador:"

/* Registration */
#define AAREGTOC "Registrado para:"
#define AANUMBERC "N�mero:"
#define AAGSVIEWREG "Registro de GSview"
#define AAREGISTERNOW "Reg�strese ahora"
#define AAONLINEREG "Registro -en l�nea-"
#define AAREG1 "Por favor, escriba su nombre y el n�mero de registro"
#define AAREG2 ""
#define AANAG1 "GSview est� registrado Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Por favor, colabore con el desarrollo de GSview"
#define AANAG3 "registr�ndose."
#define AANAG4 "El registro -en l�nea- de GSview puede hacerse en"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"

/* Installation program */
#define AAINSTALL1 "Instalaci�n de GSview"
#define AAINSTALLINTRO1 "Este asistente le ayudara durante la instalaci�n"
#define AAINSTALLINTRO2 "Si no est� realizando la instalaci�n con el archivo auto-extraible EXE, aseg�rese de tener todos los archivos listados en el archivo Readme.htm dentro del directorio en uso."
#define AAINSTALLINTRO3 "La instalaci�n usar� 20 - 30 Mb de espacio en el disco duro."
#define AAINSTALLCOPYRIGHT1 "Informaci�n del Copyright"
#define AAINSTALLCOPYRIGHT2 "GSview y AFPL Ghostscript son dos programas distintos."
#define AAINSTALLCOPYRIGHT3 "Tienen distintos propietarios."
#define AAINSTALLGSVER1 "�Qu� componentes desear�a instalar?"
#define AAINSTALLGSVER2 "Instalar GSview"
#define AAINSTALLGSVER3 "Instalar Ghostscript"
#define AAINSTALLGSVER4 "�Qu� versi�n de Ghostscript desear�a utilizar?"
#define AAINSTALLDIR1 "Elija un directorio para la instalaci�n de GSview y Ghostscript."
#define AAINSTALLDIR2 "Dentro de este directorio, GSview va a ser instalado en el subdirectorio:"
#define AAINSTALLDIR3 "Ghostscript va a ser instalado en el subdirectorio:"
#define AAINSTALLMKDIR1 "El directorio especificado no existe."
#define AAINSTALLMKDIR2 "Se va a crear un nuevo directorio."
#define AAINSTALLMISSING1 "Instalaci�n de GSview - Falta el archivo ZIP"
#define AAINSTALLMISSING2 "No se encuentra el archivo ZIP.  Inserte un disco con el archivo se�alado a continuaci�n."
#define AAINSTALLCONFIG1 "GSview y Ghostscript usar�n la variable de entorno TEMP para especificar un directorio para los archivos temporales."
#define AAINSTALLCONFIG2 "La variable TEMP no est� definida."
#define AAINSTALLCONFIG3 "La siguiente l�nea va a ser a�adida a su archivo autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Actualice su archivo autoexec.bat"
#define AAINSTALLCONFIG5 "Guarde una copia de su archivo autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview y Ghostscript usar�n la variable de entorno TEMP para especificar un directorio para los archivos temporales."
#define AAINSTALLCONFIGPM2 "La variable TEMP no est� definida, o necesita instalar EMX."
#define AAINSTALLCONFIGPM3 "La siguiente l�nea va a ser a�adida a su config.sys   'SET TEMP=C:\\'  y/o las variables PATH y LIBPATH se actualizar�n para EMX."
#define AAINSTALLCONFIGPM4 "Actualice su archivo config.sys"
#define AAINSTALLCONFIGPM5 "Guarde una copia de su archivo config.sys"
#define AAINSTALLFINISH "La instalaci�n de GSview puede comenzar.  \r\rPulse sobre Finalizar para continuar."
#define AAINSTALLUNZIP1 "Descomprimiendo archivos..."
#define AADIRNOTEXIST "El directorio '%s' no existe. �Desea crearlo?"
#define AAMKDIRFAIL "El directorio no se ha podido crear."
#define AABETAWARN "Esta versi�n BETA de GSview es de prueba. Dejar� de funcionar el %04d-%02d-%02d."
#define AAINSTALLOK "Se ha completado la instalaci�n."
#define AAINSTALLFAILED "La instalaci�n no ha podido completarse."
#define AAPROGMANGROUP1 "La instalaci�n de GSview no pudo crear un grupo de programas o un elemento del men� de Inicio."
#define AAPROGMANGROUP2 "Crear grupo / elemento"
#define AAPROGMANGROUP3 "Nombre:"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "El grupo de programas \042%s\042 ha sido creado."
#define AAPROGMANGROUP6 "El acceso directo al programa \042GSview\042 ha sido incluido en el escritorio."
#define AAPMOBJECT1 "La instalaci�n de GSview no puede incluir el acceso directo a GSview en el escritorio."
#define AAPMOBJECT2 "Incluir el acceso directo a GSview."
#define AADONEBAK "El antiguo archivo autoexec.bat ha sido guardado como %s."
#define AACANTLOAD "El archivo %s no puede ser le�do."
#define AACANTCREATETEMPFILE "No puede crearse un archivo temporal."
#define AACANTOPENREAD "No se puede leer el archivo %s."
#define AACANTOPENWRITE "No se puede escribir en el archivo %s."
#define AAERRORRENAME "Error al renombrar %s por %s."
#define AANODDEPROGMAN "No pudo establecerse la conexi�n DDE con el Administrador de Programas. Reinicie Windows."
#define AAINSERTDISK "Inserte un disco que contenga el archivo %s."
#define AAZIPNOTFOUND "No se encuentra el archivo ZIP."
#define AAUNZIPCANCELLED "Cancelada la descompresi�n.\n"
#define AACANTALLOCBUF "No hay memoria suficiente."
#define AAPROGRAMOBJECTFAILED "No pudo crearse el acceso directo al programa en el escritorio."
#define AAUNINSTALLTITLE "GSview y AFPL Ghostscript"
#define AAUNINSTALLITEM "Desinstalaci�n de GSview y AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "La instalaci�n usar� 3 Mb de espacio en su disco duro"
#define AAGSVIEWREQUIRES "GSview tambi�n usar�"
#define AAVIEWREADMEDETAILS "Lea en el archivo Readme los detalles de c�mo conseguir Ghostscript."
#define AACOPYRIGHTNOTICE "Copyright"
#define AASELECTGSVIEWDIR "Seleccione el directorio en el que se instalar� GSview"
#define AABROWSEE "Examinar..."
#define AAGSVIEWADDSTARTMENU "La instalaci�n de GSview a�adir� elementos a la siguiente carpeta del men� de Inicio"
#define AACREATEFOLDER "Crear nueva carpeta"
#define AAALLUSERS "All Users" /* la carpeta en win-profiles se llama de esta forma */
#define AASELECTFOLDER "Seleccionar carpeta"
#define AAFOLDER "Carpeta"
#define AADESTFOLDER "Carpeta de destino"
#define AAVIEWREADME "Lea el archivo Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Configuraci�n de GSview"
#define AACFG10 "Este asistente le ayudar� a configurar GSview."
#define AACFG11 "Si no entiende alg�n elemento de la p�gina siguiente,\
 no lo cambie.  Todos los valores predeterminados son sensibles."
#define AACFG20 "GSview necesita conocer qu� versi�n de Ghostscript va a usar\
 y d�nde est� ubicada."
#define AACFG21 "�Qu�  versi�n de Ghostscript desear�a utilizar?"
#define AACFG22 "�En qu� directorio est� ubicado Ghostscript?"
#define AACFG23 "Buscar fuentes adicionales de Tipo 1 en"
#define AACFG30 "GSview guarda una lista con los dispositivos de impresi�n de Ghostscript, y\
 las resoluciones que usan, en su archivo de inicializaci�n."
#define AACFG31 "La lista con los dispositivos y resoluciones est� tomada de la distribuci�n\
 est�ndar de la versi�n 5.50 de Ghostscript y puede que no est� completa."
#define AACFG32 "Actualizaci�n de la lista de impresoras GSview."
#define AACFG33 "La actualizaci�n de la lista de impresoras GSview escribir� sobre la lista existente."
#define AACFG40 "GSview puede crear asociaciones de archivo para los tipos PostScript y\
 Portable Document Format (PDF)."
#define AACFG43 "Con estas asociaciones, se iniciar� GSview al hacer doble click sobre un archivo PostScript\
 o PDF.\
  Estas asociaciones tambi�n permiten a algunos  navegadores WWW\
 utilizar GSview como visor de archivos PostScript y PDF."
#define AACFG41 "Asociar los archivos PostScript (.ps y .eps) con GSview."
#define AACFG42 "Asociar los archivos PDF (.pdf) con GSview."
#define AACFG50 "GSview puede crear una nueva carpeta en el Administrador de Programas o\
 accesos directos a GSview y AFPL Ghostscript en el men� de Inicio."
#define AACFG51 "Crear nueva carpeta / acceso directo"
#define AACFG52 "Nombre:"
#define AACFG53 "GSview puede crear un acceso directo a GSview en el escritorio.\
 Esto asociar� los archivos PostScript (.ps y .eps) y PDF (.pdf) con GSview."
#define AACFG54 "Crear acceso directo"
#define AACFG60 "La configuraci�n de GSview puede comenzar.  \r\rPulse sobre Finalizar para continuar."
#define AACFG70 "Se ha completado la configuraci�n."
#define AACFG72 "Si necesita cambiar la configuraci�n,\
 inicie  GSview y despu�s seleccione  \042Opciones | Configuraci�n sencilla...\042 o\
 \042Opciones | Configuraci�n avanzada...\042."
#define AACFG73 "La configuraci�n no ha podido completarse."
#define AACFG74 "La configuraci�n ha sido cancelada."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Impresi�n con GSview"
#define AAGSVIEWPRINTUSAGE "Uso: %s [/d] camino_dll archivo_opciones archivo_entrada\narchivo_opciones y archivo_entrada ser�n borrados al salir\ngvpgs est� programado para ser llamado con archivos temporales\n"
#ifdef _Windows
#define AAMFILE "&Archivo"
#define AAMSAVEAS "&Guardar c�mo..."
#define AAMEXIT "&Salir"
#define AAMEDIT "&Editar"
#define AAMCOPY "&Copiar\tCtrl+C"
#define AAMHELP "A&yuda"
#define AAMABOUT "Acerca de..."
#else
#define AAMFILE "~Archivo"
#define AAMSAVEAS "G~uardar c�mo..."
#define AAMEXIT "~Salir"
#define AAMEDIT "~Editar"
#define AAMCOPY "Copiar\tCtrl+C"
#define AAMHELP "A~yuda"
#define AAMABOUT "Acerca de..."
#endif


#define WINHELPFILE "gsviewes.chm"
