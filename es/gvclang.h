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

#define AAESPANOL "Español" /* Espaniol in code page 1252 */

#define GSVIEW_COPYRIGHT1 "Copyright (C) 1993-2012 Ghostgum Software Pty Ltd."
#define GSVIEW_COPYRIGHT2 "Reservados todos los derechos."
#define GSVIEW_COPYRIGHT3 "Léase el archivo LICENCE para más detalles."

#define GS_COPYRIGHT1 "Copyright (C) 2011 Artifex Software, Inc."
#define GS_COPYRIGHT2 "California, U.S.A.  Reservados todos los derechos."
#define GS_COPYRIGHT3 "Léase el archivo PUBLIC para más detalles."

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
#define AACOPY1 "Versión:"
#define AACOPY2 "Un interface gráfico para Ghostscript"
#define AACOPY4 "Este programa se distribuye SIN NINGÚN TIPO DE GARANTÍA."
#define AACOPY5 "Ni el autor ni los distribuidores aceptan ninguna responsabilidad por las"
#define AACOPY6 "consecuencias de su uso, o por si es útil para cualquier propósito"
#define AACOPY7 "o trabajo particular, salvo que él o ellos la acepten por escrito. Lea"
#define AACOPY8 "la GSview Free Public Licence (la 'Licencia') para más información."
#define AACOPY9 "Cada copia de GSview debe incluir una copia de la Licencia, distribuida"
#define AACOPY10 "normalmente en el archivo de texto ASCII llamado LICENCE.  La Licencia"
#define AACOPY11 "le permite copiar, modificar y redistribuir GSview, pero sólo bajo"
#define AACOPY12 "ciertas condiciones descritas en la Licencia. Entre otras,"
#define AACOPY13 "la Licencia requiere que la información acerca del copyright y esta nota"
#define AACOPY14 "aparezcan en todas las copias."
#define AACOPY15 "pstotxt3.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY16 "pstotxt2.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY17 "pstotxt1.dll es Copyright (C) 1995-1998 Digital Equipment Corporation"
#define AACOPY18 "y tiene una licencia particular.  Lea el archivo pstotext.txt para los detalles."
#define AACOPY19 "Autor: Russell Lang, Ghostgum Software Pty Ltd"
#define AACOPY20 "Un interface en forma DLL para Ghostscript."
#define AACOPY21 "Este programa forma parte de GSview."
#define AACOPY22 "Gestor de impresión para GSview Win32s/Win16"

#define AAINPUT "Entrada"

/* DSC error dialog box */
#define AADSC "Reglas de Estructuración de Documentos (DSC)"
#define AAIGNOREALLDSC "Ignorar comentarios DSC"

/* Info dialog box */
#define AAINFO  "Información"
#define AAFILEC "Archivo:"
#define AATYPEC "Tipo:"
#define AATITLEC "Título:"
#define AADATEC "Fecha:"
#define AABOUNDINGBOXC "BoundingBox:"
#define AAORIENTATIONC "Orientación:"
#define AADEFAULTMEDIAC "Tamaño del papel:"
#define AAPAGEORDERC "Paginación:"
#define AAPAGESC "Nº de páginas:"
#define AAPAGEC "Página:"
#define AABITMAPC "Bitmap:"

/* Sounds dialog box */
#define AASOUNDS "Sonidos"
#define AAEVENTC "Suceso:"
#define AASOUNDC "Sonido:"

/* Select port, printer, page dialog boxes */
#define AASELECTPORT "Selección del puerto de la impresora"
#define AASELECTPRINTER "Selección de la impresora"
#define AAPRINTING "Imprimiendo"
#define AASELECTPAGE "Selección de la página"
#define AASELECTPAGES "Selección de las páginas"

/* Convert and Printer Setup dialog box */
#define AAPRINTERSETUP "Configuración de la impresora"
#define AADEVICEC "Modelo:"
#define AARESOLUTIONC "Resolución:"
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
#define AAPAGES "Páginas"
#define AAFROMC "Desde:"
#define AATOC "Hasta:"

#define AAADVANCED "Avanzado"
#define AAADVANCEDPSOPT "Opciones PostScript avanzadas"
#define AASENDCTRLDBEFORE "Enviar CTRL+D antes del trabajo"
#define AASENDCTRLDAFTER "Enviar CTRL+D después del trabajo"
#define AAPROLOGFILE "Archivo prólogo"
#define AAEPILOGFILE "Archivo epílogo"
#define AABROWSE "Examinar"

/* PS2EPS dialog box */
#define AAPSTOEPS "PS a EPS"
#define AAPSTOEPSREAD "¿Ha leído la ayuda 'PS a EPS'?"
#define AAPSTOEPSAUTO "Cálculo automático del BoundingBox"

#ifdef UNIX
#define AAAPROPERTIES "Propiedades"
#define AAALLPAGES "Todas las páginas"
#define AAODDPAGES "Páginas impares"
#define AAEVENPAGES "Páginas pares"
#define AAALL "Todas"
#define AAODD "Impares"
#define AAEVEN "Pares"
#define AAREVERSE "Orden inverso"
#else
#ifdef _Windows
#define AAAPROPERTIES "P&ropiedades"
#define AAALLPAGES "&Todas las páginas"
#define AAODDPAGES "Páginas &impares"
#define AAEVENPAGES "Páginas &pares"
#define AAALL "&Todas"
#define AAODD "&Impares"
#define AAEVEN "&Pares"
#define AAREVERSE "&Orden inverso"
#else
#define AAAPROPERTIES "P~ropiedades"
#define AAALLPAGES "~Todas las páginas"
#define AAODDPAGES "Páginas ~impares"
#define AAEVENPAGES "Páginas ~pares"
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
#define AAPAGEOFFSETPT "Márgenes (pt)"
#define AAXC "X:"
#define AAYC "Y:"

/* Edit Properties dialog box */
#define AAEDITPROPERTIES "Editar propiedades"
#define AAPROPERTYTYPEC "Tipo de propiedad:"
#define AANUMBER "Número"
#define AASTRING "Texto"
#define AANAMEC "Nombre: (por ejemplo,  BitsPerPixel)"
#define AAVALUESC "Valores: (por ejemplo,  1,3,8,16,24)"
#define AADELETE "Eliminar"

#define AABOUNDINGBOX "BoundingBox"

/* Advanced Configure dialog box */
#define AACONFIGUREGS "Configuración avanzada"
#define AAGHOSTSCRIPTDLLC "Ghostscript DLL:"
#define AAGHOSTSCRIPTSOC "Objeto compartido de Ghostscript:" 
#define AAGHOSTSCRIPTEXEC "Programa Ghostscript:" 
#define AAGHOSTSCRIPTINCC "Camino de búsqueda de Ghostscript:"
#define AAGHOSTSCRIPTOTHERC "Opciones de Ghostscript:"
#define AACOPYPRINTERDEF "Copiar los valores predeterminados para la impresora"
#define AAASSOCIATEPS "Asociar los archivos .ps a GSview"
#define AAASSOCIATEPDF "Asociar los archivos .pdf a GSview"
#define AACREATESTARTMENU "Crear las entradas en el menú de Inicio"

/* Easy Configure dialog box */
#define AAEASYCONFIGURE "Configuración sencilla"
#define AAWHICHGS "¿Qué versión de Ghostscript quiere utilizar?"

/* Download Ghostscript dialog box */
#define AADOWNLOADGS "Descargar Ghostscript"
#define AADOWNLOADNOW "Descargar ahora"
#define AAGSNOTFOUND "Ghostscript no se encuentra en su ordenador.  Necesita descargar e instalar AFPL Ghostscript desde"

#define AAGSMESSWIN "GSview para Windows - Mensajes de Ghostscript"
#define AAGSMESSPM "PM GSview - Mensajes de Ghostscript"
#define AAGSMESSX11 "X11 GSview - Mensajes de Ghostscript"
#define AACOPY "Copiar"

/* Display Settings dialog box */
#define AADISPLAYSETTINGS "Parámetros de pantalla"
#define AARESOLUTION "Resolución"
#define AAZOOMRESOLUTION "Resolución del zoom"
#define AADEPTH "Intensidad"
#define AATEXTALPHA "Definición de textos"
#define AAGRAPHICSALPHA "Definición de gráficos"
#define AADRAWMETHOD "Método de dibujo"
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
#define AACALCXFORM "Cálculo de la transformación"
#define AATRANSLATE "Traslación"
#define AAROTATE "Giro"
#define AASCALE "Escala"
#define AAINITMATRIX "Matriz inicial"
#define AAINVERTMATRIX "Matriz inversa"

/* PS to Edit dialog box */
#define AAPSTOEDIT "PS a formato vectorial"
#define AAFORMATC "Formato:"
#define AADRAWTASP "Dibujar el texto usando polígonos"
#define AAMAPTOLATIN1 "Llevar a código ISO-Latin1"
#define AAFLATNESSC "Contraste:"
#define AADEFAULTFONTC "Fuente predeterminada:"
#define AADRIVEROPTIONSC "Opciones del controlador:"

/* Registration */
#define AAREGTOC "Registrado para:"
#define AANUMBERC "Número:"
#define AAGSVIEWREG "Registro de GSview"
#define AAREGISTERNOW "Regístrese ahora"
#define AAONLINEREG "Registro -en línea-"
#define AAREG1 "Por favor, escriba su nombre y el número de registro"
#define AAREG2 ""
#define AANAG1 "GSview está registrado Copyright 2006 Ghostgum Software Pty Ltd."
#define AANAG2 "Por favor, colabore con el desarrollo de GSview"
#define AANAG3 "registrándose."
#define AANAG4 "El registro -en línea- de GSview puede hacerse en"
/* Borland Resource Compiler has a bug - it can't handle "//" inside a string */
#define AANAG5 "    http:/\057www.ghostgum.com.au/"

/* Installation program */
#define AAINSTALL1 "Instalación de GSview"
#define AAINSTALLINTRO1 "Este asistente le ayudara durante la instalación"
#define AAINSTALLINTRO2 "Si no está realizando la instalación con el archivo auto-extraible EXE, asegúrese de tener todos los archivos listados en el archivo Readme.htm dentro del directorio en uso."
#define AAINSTALLINTRO3 "La instalación usará 20 - 30 Mb de espacio en el disco duro."
#define AAINSTALLCOPYRIGHT1 "Información del Copyright"
#define AAINSTALLCOPYRIGHT2 "GSview y AFPL Ghostscript son dos programas distintos."
#define AAINSTALLCOPYRIGHT3 "Tienen distintos propietarios."
#define AAINSTALLGSVER1 "¿Qué componentes desearía instalar?"
#define AAINSTALLGSVER2 "Instalar GSview"
#define AAINSTALLGSVER3 "Instalar Ghostscript"
#define AAINSTALLGSVER4 "¿Qué versión de Ghostscript desearía utilizar?"
#define AAINSTALLDIR1 "Elija un directorio para la instalación de GSview y Ghostscript."
#define AAINSTALLDIR2 "Dentro de este directorio, GSview va a ser instalado en el subdirectorio:"
#define AAINSTALLDIR3 "Ghostscript va a ser instalado en el subdirectorio:"
#define AAINSTALLMKDIR1 "El directorio especificado no existe."
#define AAINSTALLMKDIR2 "Se va a crear un nuevo directorio."
#define AAINSTALLMISSING1 "Instalación de GSview - Falta el archivo ZIP"
#define AAINSTALLMISSING2 "No se encuentra el archivo ZIP.  Inserte un disco con el archivo señalado a continuación."
#define AAINSTALLCONFIG1 "GSview y Ghostscript usarán la variable de entorno TEMP para especificar un directorio para los archivos temporales."
#define AAINSTALLCONFIG2 "La variable TEMP no está definida."
#define AAINSTALLCONFIG3 "La siguiente línea va a ser añadida a su archivo autoexec.bat\r     SET TEMP=C:\\"
#define AAINSTALLCONFIG4 "Actualice su archivo autoexec.bat"
#define AAINSTALLCONFIG5 "Guarde una copia de su archivo autoexec.bat"
#define AAINSTALLCONFIGPM1 "GSview y Ghostscript usarán la variable de entorno TEMP para especificar un directorio para los archivos temporales."
#define AAINSTALLCONFIGPM2 "La variable TEMP no está definida, o necesita instalar EMX."
#define AAINSTALLCONFIGPM3 "La siguiente línea va a ser añadida a su config.sys   'SET TEMP=C:\\'  y/o las variables PATH y LIBPATH se actualizarán para EMX."
#define AAINSTALLCONFIGPM4 "Actualice su archivo config.sys"
#define AAINSTALLCONFIGPM5 "Guarde una copia de su archivo config.sys"
#define AAINSTALLFINISH "La instalación de GSview puede comenzar.  \r\rPulse sobre Finalizar para continuar."
#define AAINSTALLUNZIP1 "Descomprimiendo archivos..."
#define AADIRNOTEXIST "El directorio '%s' no existe. ¿Desea crearlo?"
#define AAMKDIRFAIL "El directorio no se ha podido crear."
#define AABETAWARN "Esta versión BETA de GSview es de prueba. Dejará de funcionar el %04d-%02d-%02d."
#define AAINSTALLOK "Se ha completado la instalación."
#define AAINSTALLFAILED "La instalación no ha podido completarse."
#define AAPROGMANGROUP1 "La instalación de GSview no pudo crear un grupo de programas o un elemento del menú de Inicio."
#define AAPROGMANGROUP2 "Crear grupo / elemento"
#define AAPROGMANGROUP3 "Nombre:"
#define AAPROGMANGROUP4 "GS Tools"
#define AAPROGMANGROUP5 "El grupo de programas \042%s\042 ha sido creado."
#define AAPROGMANGROUP6 "El acceso directo al programa \042GSview\042 ha sido incluido en el escritorio."
#define AAPMOBJECT1 "La instalación de GSview no puede incluir el acceso directo a GSview en el escritorio."
#define AAPMOBJECT2 "Incluir el acceso directo a GSview."
#define AADONEBAK "El antiguo archivo autoexec.bat ha sido guardado como %s."
#define AACANTLOAD "El archivo %s no puede ser leído."
#define AACANTCREATETEMPFILE "No puede crearse un archivo temporal."
#define AACANTOPENREAD "No se puede leer el archivo %s."
#define AACANTOPENWRITE "No se puede escribir en el archivo %s."
#define AAERRORRENAME "Error al renombrar %s por %s."
#define AANODDEPROGMAN "No pudo establecerse la conexión DDE con el Administrador de Programas. Reinicie Windows."
#define AAINSERTDISK "Inserte un disco que contenga el archivo %s."
#define AAZIPNOTFOUND "No se encuentra el archivo ZIP."
#define AAUNZIPCANCELLED "Cancelada la descompresión.\n"
#define AACANTALLOCBUF "No hay memoria suficiente."
#define AAPROGRAMOBJECTFAILED "No pudo crearse el acceso directo al programa en el escritorio."
#define AAUNINSTALLTITLE "GSview y AFPL Ghostscript"
#define AAUNINSTALLITEM "Desinstalación de GSview y AFPL Ghostscript"

/* setup program */
#define AAINSTALLSPACE "La instalación usará 3 Mb de espacio en su disco duro"
#define AAGSVIEWREQUIRES "GSview también usará"
#define AAVIEWREADMEDETAILS "Lea en el archivo Readme los detalles de cómo conseguir Ghostscript."
#define AACOPYRIGHTNOTICE "Copyright"
#define AASELECTGSVIEWDIR "Seleccione el directorio en el que se instalará GSview"
#define AABROWSEE "Examinar..."
#define AAGSVIEWADDSTARTMENU "La instalación de GSview añadirá elementos a la siguiente carpeta del menú de Inicio"
#define AACREATEFOLDER "Crear nueva carpeta"
#define AAALLUSERS "All Users" /* la carpeta en win-profiles se llama de esta forma */
#define AASELECTFOLDER "Seleccionar carpeta"
#define AAFOLDER "Carpeta"
#define AADESTFOLDER "Carpeta de destino"
#define AAVIEWREADME "Lea el archivo Readme"

/* configure */
#define AAGSVIEWC "GSview:"
#define AAALADDINGSC "AFPL Ghostscript:"
#define AACFG1 "Configuración de GSview"
#define AACFG10 "Este asistente le ayudará a configurar GSview."
#define AACFG11 "Si no entiende algún elemento de la página siguiente,\
 no lo cambie.  Todos los valores predeterminados son sensibles."
#define AACFG20 "GSview necesita conocer qué versión de Ghostscript va a usar\
 y dónde está ubicada."
#define AACFG21 "¿Qué  versión de Ghostscript desearía utilizar?"
#define AACFG22 "¿En qué directorio está ubicado Ghostscript?"
#define AACFG23 "Buscar fuentes adicionales de Tipo 1 en"
#define AACFG30 "GSview guarda una lista con los dispositivos de impresión de Ghostscript, y\
 las resoluciones que usan, en su archivo de inicialización."
#define AACFG31 "La lista con los dispositivos y resoluciones está tomada de la distribución\
 estándar de la versión 5.50 de Ghostscript y puede que no esté completa."
#define AACFG32 "Actualización de la lista de impresoras GSview."
#define AACFG33 "La actualización de la lista de impresoras GSview escribirá sobre la lista existente."
#define AACFG40 "GSview puede crear asociaciones de archivo para los tipos PostScript y\
 Portable Document Format (PDF)."
#define AACFG43 "Con estas asociaciones, se iniciará GSview al hacer doble click sobre un archivo PostScript\
 o PDF.\
  Estas asociaciones también permiten a algunos  navegadores WWW\
 utilizar GSview como visor de archivos PostScript y PDF."
#define AACFG41 "Asociar los archivos PostScript (.ps y .eps) con GSview."
#define AACFG42 "Asociar los archivos PDF (.pdf) con GSview."
#define AACFG50 "GSview puede crear una nueva carpeta en el Administrador de Programas o\
 accesos directos a GSview y AFPL Ghostscript en el menú de Inicio."
#define AACFG51 "Crear nueva carpeta / acceso directo"
#define AACFG52 "Nombre:"
#define AACFG53 "GSview puede crear un acceso directo a GSview en el escritorio.\
 Esto asociará los archivos PostScript (.ps y .eps) y PDF (.pdf) con GSview."
#define AACFG54 "Crear acceso directo"
#define AACFG60 "La configuración de GSview puede comenzar.  \r\rPulse sobre Finalizar para continuar."
#define AACFG70 "Se ha completado la configuración."
#define AACFG72 "Si necesita cambiar la configuración,\
 inicie  GSview y después seleccione  \042Opciones | Configuración sencilla...\042 o\
 \042Opciones | Configuración avanzada...\042."
#define AACFG73 "La configuración no ha podido completarse."
#define AACFG74 "La configuración ha sido cancelada."


/* For gvXgs.rc */
#define AAGSVIEWPRINT "Impresión con GSview"
#define AAGSVIEWPRINTUSAGE "Uso: %s [/d] camino_dll archivo_opciones archivo_entrada\narchivo_opciones y archivo_entrada serán borrados al salir\ngvpgs está programado para ser llamado con archivos temporales\n"
#ifdef _Windows
#define AAMFILE "&Archivo"
#define AAMSAVEAS "&Guardar cómo..."
#define AAMEXIT "&Salir"
#define AAMEDIT "&Editar"
#define AAMCOPY "&Copiar\tCtrl+C"
#define AAMHELP "A&yuda"
#define AAMABOUT "Acerca de..."
#else
#define AAMFILE "~Archivo"
#define AAMSAVEAS "G~uardar cómo..."
#define AAMEXIT "~Salir"
#define AAMEDIT "~Editar"
#define AAMCOPY "Copiar\tCtrl+C"
#define AAMHELP "A~yuda"
#define AAMABOUT "Acerca de..."
#endif


#define WINHELPFILE "gsviewes.chm"
