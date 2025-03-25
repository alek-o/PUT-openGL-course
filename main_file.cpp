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
#define FINGER_SPEED 1

//float speed = PI; //Prędkość kątowa obrotu obiektu
float speed = 0; //Prędkość kątowa obrotu obiektu
float finger_speed = 0;

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
		if (key == GLFW_KEY_UP) finger_speed = FINGER_SPEED;
		if (key == GLFW_KEY_DOWN) finger_speed = -FINGER_SPEED;
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_UP) finger_speed = 0;
		if (key == GLFW_KEY_DOWN) finger_speed = 0;
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
void drawScene(GLFWwindow* window, float angle, float finger_angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyszczenie bufora kolorów i bufora głębokości


	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wyliczenie macierzy widoku

	spLambert->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załadowanie macierzy widoku do programu cieniującego
	
	glm::mat4 MA = glm::mat4(1.0f); // Arm Matrix
	MA = glm::rotate(MA, angle, glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 MCore = MA; // Macierz podłoża
	MCore = glm::translate(MCore, glm::vec3(0.0f, 0.0f, 0.0f));
	MCore = glm::scale(MCore, glm::vec3(1.0f, 0.5f, 1.0f));
	glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MCore));  //Załadowanie macierzy modelu do programu cieniującego
	glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
	Models::cube.drawSolid();
	
	for (int i = 0; i < 4; i++)
	{
		float finger_number = (float)i / 4 * 2 * PI;

		glm::mat4 MF1P = MA;
		MF1P = glm::rotate(MF1P, finger_number, glm::vec3(0.0f, 1.0f, 0.0f));
		MF1P = glm::translate(MF1P, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 MF1 = MF1P; // Macierz podłoża
		MF1 = glm::rotate(MF1, finger_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MF1 = glm::translate(MF1, glm::vec3(2.0f, 0.0f, 0.0f));
		MF1 = glm::scale(MF1, glm::vec3(2.0f, 0.5f, 1.0f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MF1));  //Załadowanie macierzy modelu do programu cieniującego
		glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
		Models::cube.drawSolid();

		glm::mat4 MF1GP = MF1P;
		MF1GP = glm::rotate(MF1GP, finger_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MF1GP = glm::translate(MF1GP, glm::vec3(4.0f, 0.0f, 0.0f));

		glm::mat4 MF1G = MF1GP; // Macierz podłoża
		MF1G = glm::rotate(MF1G, finger_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MF1G = glm::translate(MF1G, glm::vec3(2.0f, 0.0f, 0.0f));
		MF1G = glm::scale(MF1G, glm::vec3(2.0f, 0.5f, 1.0f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MF1G));  //Załadowanie macierzy modelu do programu cieniującego
		glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
		Models::cube.drawSolid();

		glm::mat4 MF1GP2 = MF1GP;
		MF1GP2 = glm::rotate(MF1GP2, finger_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MF1GP2 = glm::translate(MF1GP2, glm::vec3(4.0f, 0.0f, 0.0f));

		glm::mat4 MF1G2 = MF1GP2; // Macierz podłoża
		MF1G2 = glm::rotate(MF1G2, finger_angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MF1G2 = glm::translate(MF1G2, glm::vec3(2.0f, 0.0f, 0.0f));
		MF1G2 = glm::scale(MF1G2, glm::vec3(2.0f, 0.5f, 1.0f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MF1G2));  //Załadowanie macierzy modelu do programu cieniującego
		glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
		Models::cube.drawSolid();
	}


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
	float finger_angle = 0;
	glfwSetTime(0); //Wyzeruj timer
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle += speed * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		finger_angle += finger_speed * glfwGetTime();
		glfwSetTime(0); //Wyzeruj timer
		drawScene(window, angle, finger_angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.

	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}