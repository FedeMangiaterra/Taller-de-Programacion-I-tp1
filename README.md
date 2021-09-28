# Taller de Programación I
## Ejercicio N°1: Ahorcado
* Nombre: Federico Carlos Mangiaterra 
* Padrón: 105771 
* Link del repositorio de Github: https://github.com/FedeMangiaterra/Taller-de-Programacion-I-tp1
## Introducción
Este trabajo consiste en la implementación en C del conocido juego del ahorcado, con la característica principal de que el juego se realiza entre un servidor y uno o varios clientes, dependiendo del caso. Y esto se consigue mediante la utilización de sockets. Otro punto clave del trabajo es el lograr desarrollar TDAs encapsulados correctamente, respetando qué debe y qué no debe "saber" cada uno de ellos. En este caso, se implementaron 4 TDAs:
* Hangman Game (juego del ahorcado)
* Socket
* Client Hangman
* Server Hangman
## Resolución del trabajo
### Hangman Game
Inicialmente, lo que se hizo fue programar la lógica del ahorcado, la cual no es muy compleja. A partir de una palabra y una cantidad de intentos, empieza a recibir letras, y busca si hay coincidencias entre las mismas y la palabra que se debe adivinar. Esto se consigue gracias a la funcion *strchr()* de la biblioteca "string.h", que devuelve un puntero a la primera coincidencia entre la palabra y la letra, o NULL si no existe. Tras esa comprobacion, si hay coincidencia, se recorre la palabra y al llegar a posiciones donde está la letra, esta se reemplaza en la misma posicion pero de la partial word*, que es donde se guarda el progreso del jugador. Si no había coincidencias, se reduce en 1 a cantidad de intentos. <br>
Después, para modularizar bien, se encapsuló todo en el TDA hangman game, con sus respectivas funciones de *init* y *uninit*.
### Socket
Lo siguiente fue la implementación de un TDA socket, usando como guía un archivo .h mostrado por la cátedra. La mayor ventaja de este TDA es que facilita muchísimo para el usuario el manejo de sockets, porque se ahorra el uso de funciones complejas como *getaddrinfo()* (ésta además usa memoria dinámica lo cual podría traer más fallos) al tenerlas dentro de las funciones de *socket_bind_and_listen()* y *socket_connect*. Además, se incluyen comprobaciones de errores en las diferentes funciones, imprimiendo mensajes que indican en dónde hubo algún fallo. Otra cosa a destacar son las funciones de *socket_send* y *socket_receive* de este TDA, porque se aseguran de que se envíen y se reciban las cantidades de bytes indicadas, haciendo uso de ciclos while que no se rompen hasta que se cumpla con lo pedido.
### Vínculo entre Hangman y Socket
Como se mencionó antes, en este trabajo se busca hacer un servidor para jugar al ahorcado, pero el ahorcado en sí no implica la existencia de un servidor en el cual jugarse, por lo que se optó por agregarle varios parametros más a la función de *hangman_game_play()*, con diferentes callbacks y callback_ctx. La función de estos es la de darle la libertad a quien usa el TDA hangman game de darle cierto grado de personalización al juego, con funciones para obtener las letras con las cuales jugar, para hacer algo con los datos del desarrollo de la partida (la palabra parcialmente adivinada y los intentos restantes en el momento), y para hacer algo una vez que se adivinó la palabra o se terminaron los intentos. Igualmente, se permite la posibilidad de que el usuario no quiera tener que programar sus propias funciones, ya que en caso de que no se pasen funciones como parámetro, el comportamiento del juego es el implementado originalmente. <br>
Haciendo uso de esta opción de personalización, se pueden implementar las funciones que vinculen al juego con el socket, sin romper con el encapsulamiento.
### Server y Client
Lo último fue unir lo anterior para poder armar el servidor y el cliente, cada uno con su respectivo comportamiento. <br>
Para el server, siguiendo con lo pedido en el trabajo, se inicializa un socket de tipo server y lo que se hace es recorrer un archivo de texto con palabras separadas por saltos de línea. Para obtener las palabras hace uso de la función *getline()*, la cual usa memoria dinámica, por lo que se incluyen sus respectivos llamados a free() para que no haya pérdidas. Mientras queden palabras, el proceso siempre es el mismo, espera a que se quieran conectar y cuando se establece la conexión se inicia una partida de ahorcado. Es acá donde se hace uso de las funciones callback para lograr el objetivo de recibir la letra por parte del cliente y enviarle a este la información de la partida. Las funciones de *obtain_letter()* y *send_hangman_data()*.<br>
La primera es muy simple, sólo hace un receive de un byte: la letra, y la devuelve. <br>
La segunda hace uso del protocolo de comunicación detallado en la consigna del trabajo, donde se envía en el primer byte los intentos restantes y si se terminó o no la partida, en los siguientes 2 el largo de la palabra con formato big endian y en los restantes la palabra parcial. Para el primer byte usa un unsigned char. Como se indica que el primer bit debe ponerse en 1 si la partida terminó, lo que se hizo fue sumarle 128 (2^7, el valor del bit mas significativo en 8 bits). Esto tambien limita la cantidad maxima de intentos a 127, porque se tienen solo 7 bits destinados a almacenar esa informacion. Para asegurar el tamaño en bytes del largo de la palabra, se usa el tipo uint16_t, porque su tamaño no varía según la implementación. Por otro lado, la endianess correcta se logra usando la funcion *htons()*, lo cual significa *host to network short*. Cambia el orden de almacenamiento de los bytes de un short por el de la red, la cual es big endian. Esto quiere decir que los bytes se van a enviar en el orden requerido aunque el host almacene las cosas en formato little endian. <br>
Luego de completar el buffer con toda la información correctamente ordenada, la envía. Esta función también se usa al finalizar la partida, pero aunque el cliente no haya adivinado, se envía la palabra y no la palabra parcial. <br>
Cuando termina la partida, ademas de los uninits del socket de la conexion entre host y cliente y de la partida de ahorcado, se aumenta el contador de victorias o derrotas del servidor. Estos resultados se muestran cuando la funcion *getline()* devuelve -1, porque eso significa que se llego al EOF (o que hubo otro error).<br>
Todo esto terminó conformando el TDA server hangman. <br>
Para el cliente, no se usa el TDA hagman_game pero sí el de socket. Se inicia uno y se establece la conexion con el servidor. Lo que hace es recibir los bytes del protocolo de comunicación e interpretarlos, para luego imprimirlos con el formato indicado. Luego solicita que se ingrese una letra. El usuario también puede ingresar más de una y éstas se procesan individualmente. Se estableció como límite a la entrada del usuario una constante arbitraria de 30, pero podría tener cualquier otro valor. Con cada letra lo que hace es enviarla al servidor y volver a interpretar lo que el servidor le manda. Este ciclo continúa hasta que la cantidad de intentos sea mayor o igual a 128, pues esto significa que el primer bit del unsigned char que lo almacena está en 1. Luego se fija qué pasa si a ese numero le resta 128. Si es 0, es porque el cliente se quedó sin intentos. en caso contrario, el cliente adivinó la palabra. Dependiendo de esto ultimo imprime uno u otro mensaje de fin de partida, con la particularidad de que si el cliente perdió, se hace uso de el recibir la palabra correcta por parte del servidor para poder mostrarla en pantalla.
