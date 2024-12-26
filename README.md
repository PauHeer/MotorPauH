# SpectraEngine

## Descripción
Es un motor de videojuegos 3D en desarrollo en **C++** con **OpenGL**, capaz de renderizar geometría con diferentes controles y pantallas de información.

## Miembros del equipo
- [Pau Hernández Vázzquez](https://github.com/pauheer)

## Controles del visor
- **Clic derecho**: mover la vista.
- **Clic derecho + WASD**: volar alrededor.
- **Alt + clic derecho o rueda del mouse**: acercar o alejar.
- **Alt + clic izquierdo**: orbita la cámara alrededor.
- **Alt + rueda del mouse**: mover la cámara.
- **F**: encuadrar el objeto seleccionado.

## Características
- **Importación de modelos y texturas**: importar modelos en formato FBX y texturas en PNG y DDS.
- **Importación de texturas**: admite los formatos de texturas PNG y DDS.
- **Arrastrar y soltar**: permite importar archivos arrastrándolos directamente.
- **Controles de transformación**: aplica transformaciones como posición, rotación y escala a los modelos.
- **Sistema de registro**: proporciona un registro detallado.
- **Modos de sombreado**: cambia entre las opciones Sombreado, Estructura alámbrica y Estructura alámbrica sombreada.
- **Monitor del motor**: visualiza la información de monitoreo.
- **Biblioteca de archivos personalizados**: administra archivos con un formato de archivo personalizado.

## Paneles
- **Jerarquía**: muestra los objetos del juego presentes actualmente en la escena. Incluye una función de búsqueda para localizar objetos específicos del juego y brinda opciones para crear objetos de juego vacíos y formas primitivas básicas.
- **Inspector**: permite ver y editar las propiedades de un objeto de juego seleccionado actualmente, como opciones de transformación, renderizador de malla y material.
- **Escena**: permite visualizar e interactuar con el mundo del editor.
- **Proyecto**: permite acceder a varias carpetas de proyectos, administrar archivos e importar activos. Muestra todos los activos dentro del proyecto, incluidos modelos, texturas y otros recursos del motor.
- **Consola**: permite ver errores, advertencias e información relacionada con el motor.
- **Preferencias**: ofrece configuraciones para el usuario, incluidas opciones de pantalla, cuadrícula y renderizado.
- **Rendimiento**: muestra información sobre el rendimiento del motor e informacion de los recursos del sistema.
- **Acerca de**: muestra información básica sobre el motor.

## Bibliotecas y dependencias
- [SDL2](https://github.com/libsdl-org/SDL): biblioteca de desarrollo multiplataforma.
- [OpenGL](https://www.opengl.org/): API de gráficos para renderizar gráficos.
- [ImGui](https://github.com/ocornut/imgui): interfaz gráfica de usuario de modo inmediato para C++.
- [GLM](https://github.com/g-truc/glm): biblioteca matemática de C++ para software de gráficos basado en OpenGL.
- [Assimp](https://github.com/assimp/assimp): biblioteca para cargar varios formatos de archivos 3D en un formato inmediato compartido en memoria.
- [DeviL](https://github.com/DentonW/DevIL): biblioteca de imágenes que permite cargar, guardar y manipular varios formatos de imágenes.