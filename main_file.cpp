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

//float speed = PI; //Prędkość kątowa obrotu obiektu
float speed = 0; //Prędkość kątowa obrotu obiektu
static Models::Sphere sun(0.5, 36, 36);
static Models::Sphere planet1(0.2, 36, 36);
static Models::Sphere moon1(0.1, 36, 36);
static Models::Sphere planet2(0.25, 36, 36);
static Models::Sphere moon2(0.07, 36, 36);

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
void drawScene(GLFWwindow* window, float angle) {
	//************Tutaj umieszczaj kod rysujący obraz******************l
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Wyczyszczenie bufora kolorów i bufora głębokości


	glm::mat4 P = glm::perspective(glm::radians(50.0f), 1.0f, 1.0f, 50.0f); //Wyliczenie macierzy rzutowania
	glm::mat4 V = glm::lookAt(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f)); //Wyliczenie macierzy widoku

	spLambert->use();//Aktywacja programu cieniującego
	glUniformMatrix4fv(spLambert->u("P"), 1, false, glm::value_ptr(P)); //Załadowanie macierzy rzutowania do programu cieniującego
	glUniformMatrix4fv(spLambert->u("V"), 1, false, glm::value_ptr(V)); //Załadowanie macierzy widoku do programu cieniującego
	
	glm::mat4 MT = glm::mat4(1.0f);
	MT = glm::rotate(MT, -PI / 6, glm::vec3(1.0f, 0.0f, 0.0f));
	
	for (int i = 0; i < 6; i++)
	{
		glm::mat4 MTi = MT;
		MTi = glm::rotate(MTi, PI / 3 * i, glm::vec3(0.0f, 1.0f, 0.0f));
		MTi = glm::translate(MTi, glm::vec3(0.0f, 0.0f, 1.0f));
		MTi = glm::rotate(MTi, (i%2*2 - 1) * angle, glm::vec3(0.0f, 0.0f, 1.0f));
		MTi = glm::scale(MTi, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MTi));  //Załadowanie macierzy modelu do programu cieniującego
		glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
		Models::torus.drawWire();

		for (int j = 0; j < 12; j++)
		{
			glm::mat4 MT1Ki = MTi;
			MT1Ki = glm::rotate(MT1Ki, PI / 6 * j + (i % 2)*PI/12, glm::vec3(0.0f, 0.0f, 1.0f));
			MT1Ki = glm::translate(MT1Ki, glm::vec3(0.0f, 1.0f, 0.0f));
			MT1Ki = glm::scale(MT1Ki, glm::vec3(0.1f, 0.1f, 0.1f));
			glUniformMatrix4fv(spLambert->u("M"), 1, false, glm::value_ptr(MT1Ki));  //Załadowanie macierzy modelu do programu cieniującego
			glUniform4f(spLambert->u("color"), 0.3f, 0.0f, 0.3f, 1.0f);
			Models::cube.drawSolid();
		}
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
	glfwSetTime(0); //Wyzeruj timer
	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle += speed * glfwGetTime(); //Oblicz przyrost kąta po obrocie
		glfwSetTime(0); //Wyzeruj timer
		drawScene(window, angle); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}