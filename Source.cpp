#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <windows.h>
#include <MMSystem.h>


using namespace cv;
using namespace std;

int main(int arge, char*argv[]) {
	int porcentaje, contador = 0;

	//Se crea un objeto videocapture para poder tomar la foto
	VideoCapture cap(0);
	if (!cap.isOpened())
		return -1;

	namedWindow("webcam");
	Mat frame;// = imread("C:\\Users\\Eder\\Desktop\\Sin título.png");

	//Este for basicamente es infinito, y a va estar mandando a la ventana webcam un objeto frame que va a tomar
	// de la camara web de la computadora
	for (;;){
		cap >> frame;
		imshow("webcam", frame);
		if (waitKey(50) >= 0) break;
		//El for se va a detener cuando se presione ENTER, que básicamente podríamos decir que se termina cuando
		// se toma la foto
	}
	
	//Se muestra la foto
	imshow("FOTO", frame);
	cvWaitKey(0);

	//Vamos a convertir la foto que tomamamos que es tipo MAT a iplimage para modificar mas facil los pixeles.
	IplImage* image2;
	//la image2 se hace del mismo tamaño que la foto
	image2 = cvCreateImage(cvSize(frame.cols, frame.rows), 8, 3);
	IplImage ipltemp = frame;
	cvCopy(&ipltemp, image2);
	//se copia la foto

	//Aquí obtenemos la cantidad de pixeles de la foto que equivalen al 10 porciento
	//Ej, si es una foto de 10x10 = 100 pixeles, entonces 10% = 10 pixeles
	porcentaje = image2->width*image2->height;
	porcentaje = porcentaje *.5;

	//Aquí lo que hacemos es crear dos iplimage del mismo tamaño que la foto que tomamos y ambas las pasaremos
	// a escala de grises para poder obtener los valores minimos y maximos mas adelante
	IplImage *g,*g1;
	g = cvCreateImage(cvSize(image2->width, image2->height), IPL_DEPTH_8U, 1);
	//funcion para pasar de RGB->GRAYSCALE
	//argumentos(imagen a transformar, destino, tipo de conversion)
	cvCvtColor(image2,g, CV_BGR2GRAY);
	g1 = cvCreateImage(cvSize(image2->width, image2->height), IPL_DEPTH_8U, 1);


	//un tipo de dato CvPoint guarda las coordenadas de un pixel, es una estructura con un dato x y un dato y.
	CvPoint p1, p2;
	double d1, d2;
	//cvminmaxloc obtiene de una imagen a escala de grises (por eso convertimos nuestras fotos), el pixel minimo, el maximo y sus posiciones
	//argumentos(imagen, direccion de un dato double donde se guarda el minimo, dir double del maximo, cvpoint minimo, cvpoint maximo)
	cvMinMaxLoc(g, &d1, &d2, &p1, &p2);
	int row = 0, col = 0, a;
	//En el ciclo iremos desde 0 hasta convertir los n pixeles que son el promedio (el 10 %)
	for (a = 0; a < porcentaje; a++) {
		//Primero obtendremos el valor maximo, que seria el pixel mas claro de la imagen
		cvMinMaxLoc(g, &d1, &d2, &p1, &p2);
		//Este if con los fors aninados es para cuando suceda el caso de que el elemento mas claro restante sea el 0
		//Si se queda como elemento mayor 0 ya no tiene sentido la siguiente instruccion que pasaba el
		//valor mayor a 0, ya que lo dejaría igual o pues ya no cambiaría mas pixeles
		//Lo que hace es recorrer toda la imagen y en los valores ==0 los convierte a 1, así podemos seguir
		//cambiando los valores. Ya que ahora el mas claro sería el 1 y ahora si tendría sentido pasarlos a 0
		if (d2 == 0) {
			for (row = 0; row < g->height; row++) {
				for (col = 0; col < g->width; col++) {
					if (CV_IMAGE_ELEM(image2, uchar, row, col*3) == 0 && CV_IMAGE_ELEM(g, uchar, row, col * 3+1) == 0 && CV_IMAGE_ELEM(g, uchar, row, col * 3+2) == 0) {
						CV_IMAGE_ELEM(g, uchar, row, col) = 1;
					}
				}
			}
		}

		//Despues modificaremos con cv_image_elem el valor del pixel para que ya no sea el mas claro y en la sig iteracion
		// ya no lo vuelva a tomar
		//NOTESE QUE PRIMERO VA LA COORDENADA DE Y Y LUEGO DE X, OpenCv así lo maneja
		CV_IMAGE_ELEM(g, uchar, p2.y, p2.x) = 0;

		//COn las coordeanadas que encontramos con cvminmaxloc, modificamos los valores en la imagen RGB
		//NOTESE QUE OPENCV UTILIZA BLUE|GREEN|RED
		CV_IMAGE_ELEM(image2, uchar, p2.y, p2.x * 3) = 0;
		CV_IMAGE_ELEM(image2, uchar, p2.y, p2.x * 3 + 1) = 255;
		CV_IMAGE_ELEM(image2, uchar, p2.y, p2.x * 3 + 2) = 255;
	}
	//Se muestra la imagen
	cvShowImage("Pixeles Amarillos", image2);
	cvWaitKey(0);
	
	//Habia un error anteriormente, antes convertia las dos imagenes a escala de grises seguidas,
	//Pero la imagen con los pixeles azules resulta de la segunda imagen de los pixeles amarillos
	//Así que hay que convertir la imagen que ya tiene los pixeles amarillos y grayscale para que haga
	//El proceso correcto
	cvCvtColor(image2, g1, CV_BGR2GRAY);

	//Con este ciclo ahora se cambiaran los valores mas oscuros a azueles.
	//Realmente es el mismo rpoceso que para los amarillos, simlpemente ahora trabajamos
	//con las coordenadas del valor minimo y modificamos el mismo a 255 para que ya no vuelva a tomar
	cvMinMaxLoc(g1, &d1, &d2, &p1, &p2);
	for (a = 0; a < porcentaje; a++) {
		cvMinMaxLoc(g1, &d1, &d2, &p1, &p2);
		CV_IMAGE_ELEM(g1, uchar, p1.y, p1.x) = 255;
		CV_IMAGE_ELEM(image2, uchar, p1.y, p1.x * 3) = 255;
		CV_IMAGE_ELEM(image2, uchar, p1.y, p1.x * 3 + 1) = 0;
		CV_IMAGE_ELEM(image2, uchar, p1.y, p1.x * 3 + 2) = 0;

	}
	//Se muestra la imagen
	cvShowImage("Pixeles Azules",image2);
	cvWaitKey(0);

	//Antes de cambiar los pixeles convertimos de mat a ipl, ahora haremos el proceso inverso para las sig tareas
	frame = cvarrToMat(image2);

	//Creamos nuevos objetos mat que contendran un rectangulo de la imagen con la que trabajamos
	//Esto se hace por medio de la funcion Rect: argumentos (posicion x, posicion y, cuanto nos moveremos en x para tomar el rectangulo, cuando en y)
	// Y clonamos al objeto mat ese rectangulo de la imagen
	//Se hace 4 veces para cuatro mat distintas para obtener las 4 partes
	//NOTESE QUE LOS ARGUMENTOS CAMBIAN PARA TOMAR LA PARTE CORRECTA
	Mat image_temp1 = frame(Rect(0, 0, frame.cols/2, frame.rows / 2)).clone();

	Mat image_temp2 = frame(Rect(frame.cols / 2, 0, frame.cols/2,frame.rows/2)).clone();

	Mat image_temp3 = frame(Rect(0, frame.rows / 2, frame.cols/2, frame.rows / 2)).clone();

	Mat image_temp4 = frame(Rect(frame.cols/2, frame.rows/2, frame.cols/2, frame.rows / 2)).clone();

	//El point2f nos sirve para saber en base a que punto rotaremos la imagen
	Point2f src_center(image_temp1.cols / 2.0F, image_temp1.rows / 2.0F);
	//Nos ayuda a establecer como será la rotación de la imagen
	Mat rot_mat = getRotationMatrix2D(src_center, 180, 1.0);
	//Objetos mat para guardar las imagenes rotadas y para juntarlas en una sola
	Mat img1rotate, img4rotate, i12,i34,i;

	//warpaffine hace la rotacion de la imagen y la guarda
	//argumentos(imagen a rotar, donde se guarda, la rotacion que se hará, y el tamaño final de la imagen)
	warpAffine(image_temp1, img1rotate, rot_mat, image_temp1.size());
	warpAffine(image_temp4, img4rotate, rot_mat, image_temp4.size());

	//Concatenamos las imagenes
	//Primero concatenamos horizontalmente el primer cuadrante rotada y el segundo no rotado
	//hconcat argumentos(imagen 1 a pegar, imagen 2 a pegar, imagen donde se pegan)
	hconcat(img1rotate, image_temp2, i12);
	//despues concatenamos horizontalmente el tercer cuadrante no rotado y el cuarto rotado rotado
	//hconcat argumentos(imagen 1 a pegar, imagen 2 a pegar, imagen donde se pegan)
	hconcat(image_temp3, img4rotate, i34);
	//por ultimo concatenamos verticalmente la mitad de arriba que concatenamos el primer paso, y la mitad de abajo del segundo paso
	//vconcat argumentos(imagen 1 a pegar, imagen 2 a pegar, imagen donde se pegan)
	vconcat(i12, i34, i);

	//Se muestra la imagen
	imshow("Cuadriculada y rotada",i);
	cvWaitKey(0);


	//no necesita epxlicacion:v
	Mat gray;
	cvtColor(i, gray, CV_BGR2GRAY);
	imshow("GRAY", gray);
	
	//Este for que reproduce un Beep, literal, un beep, es el plan b si no se puede reproducir una cancion
	//Ya que, al ser un programa de 32 bits en cmd, no hay tal cosa como instalarlo y tener la cancion en una
	//carpeta en todas las laptops. POr lo que, al querer reproducir la cancion, pues no la reproduciria
	//Y el beep, pues es un sonido que viene en windows.h, y pues si lo reproduciría. Y pues tambien cumple
	//con la condicion de que al mostrar la 5ta imagen se reproduzca un sonido
	for (;;) {
		Beep(523, 500);
		if (waitKey(50) >= 0) break;
	}

	cvWaitKey(0);
	return 0;
}