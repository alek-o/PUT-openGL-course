/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "allmodels.h"
#include "lodepng.h"
#include "shaderprogram.h"

#define SPEED 1
#define TURN_SPEED 1

//float speed = PI; //Prędkość kątowa obrotu obiektu
float speed = 0; //Prędkość kątowa obrotu obiektu
float turn = 0;
static Models::Sphere sun(0.5, 36, 36);
static Models::Sphere planet1(0.2, 36, 36);
static Models::Sphere moon1(0.1, 36, 36);
static Models::Sphere planet2(0.25, 36, 36);
static Models::Sphere moon2(0.07, 36, 36);
static Models::Torus kolo(0.3, 0.1, 12, 12);

void key_callback(GLFWwindow* window, int key,
	int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_RIGHT) speed = SPEED;
		if (key == GLFW_KEY_LEFT) speed = -SPEED;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_RIGHT) speed = 0;
		if (key == GLFW_KEY_LEFT) speed = 0;
	}

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_A) turn = 1;
		if (key == GLFW_KEY_D) turn = -1;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) turn = 0;
		if (key == GLFW_KEY_D) turn = 0;
	}
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	//Rejestracja procedury callback:
	glfwSetKeyCallback(window, key_callback);
	glEnable(GL_DEPTH_TEST);
	initShaders();
	glClearColor(1, 0.3, 1, 1);//Ustaw czarny kolor czyszczenia ekranu
	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
}


//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();
	//************Tutaj umieszczaj kod, który należy wykonać po zakończeniu pętli głównej************
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle, float wheel_angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyszczenie bufora kolorów i bufora głębokości


	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wyliczenie macierzy widoku

	spLambert->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załadowanie macierzy widoku do programu cieniującego
	
	glm::mat4 MS = glm::mat4(1.0f); // Macierz samochodu
	MS = glm::rotate(MS, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	
	glm::mat4 MP = MS; // Macierz podłoża
	MP = glm::translate(MP, glm::vec3(0.0f, 0.0f, 0.0f));
	MP = glm::scale(MP, glm::vec3(1.5f, 0.3f, 1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MP));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	Models::cube.drawSolid();

	glm::mat4 MK1 = MS; // Macierz koła
	MK1 = glm::translate(MK1, glm::vec3(1.5f, 0.0f, 1.0f));
	MK1 = glm::rotate(MK1, turn, glm::vec3(0.0f, 1.0f, 0.0f));
	MK1 = glm::rotate(MK1, wheel_angle, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MK1));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	kolo.drawWire();

	glm::mat4 MK2 = MS; // Macierz koła
	MK2 = glm::translate(MK2, glm::vec3(1.5f, 0.0f, -1.0f));
	MK2 = glm::rotate(MK2, turn, glm::vec3(0.0f, 1.0f, 0.0f));
	MK2 = glm::rotate(MK2, wheel_angle, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MK2));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	kolo.drawWire();

	glm::mat4 MK3 = MS; // Macierz koła
	MK3 = glm::translate(MK3, glm::vec3(-1.5f, 0.0f, 1.0f));
	MK3 = glm::rotate(MK3, wheel_angle, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MK3));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	kolo.drawSolid();

	glm::mat4 MK4 = MS; // Macierz koła
	MK4 = glm::translate(MK4, glm::vec3(-1.5f, 0.0f, -1.0f));
	MK4 = glm::rotate(MK4, wheel_angle, glm::vec3(0.0f, 0.0f, -1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MK4));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	kolo.drawSolid();

	
	//Skopiowanie bufora ukrytego do widocznego. Z reguły ostatnie polecenie w procedurze drawScene.
	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące


	float angle = 0; //Aktualny kąt obrotu obiektu
	float wheel_angle = 0;
	glfwSetTime(0); //Wyzeruj timer
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		wheel_angle += TURN_SPEED * glfwGetTime();
		angle += speed * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		glfwSetTime(0); //Wyzeruj timer
		drawScene(window, angle, wheel_angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.

	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}