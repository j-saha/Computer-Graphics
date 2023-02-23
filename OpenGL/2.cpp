#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<iostream>
#include <windows.h>
#include <GL/glut.h>
#include <time.h>

#define pi (2*acos(0.0))
#define UNIT_MOVE 2
#define UNIT_ROTATE 3


using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;


int wheel_theta, rotate_degree;
double radius;

struct point
{
	double x,y,z;
};

point pos_wheel;



double radian(double degree)
{
    return degree*pi/180;
}



void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}










void drawCylinder(double radius,int seg)
{
	struct point points[1000];
	double height = 16;
	double shade;



	//generate points
	for(int i=0;i<=seg;i++)
	{

			points[i].x=radius*cos(((double)i/(double)seg)*2*pi);
			points[i].y=radius*sin(((double)i/(double)seg)*2*pi);
			points[i].z=height/2;

	}
	//draw quads using generated points


		for(int j=0;j<seg;j++)
		{
		            //create shading effect
            if(j<seg/2)shade=2*(double)j/(double)seg;
            else shade=2*(1.0-(double)j/(double)seg);
            glColor3f(shade,shade,shade);


		            //glColor3f((double)1/(double)stacks,(double)1/(double)stacks,(double)1/(double)stacks);

			glBegin(GL_QUADS);{

			    //upper hemisphere
				glVertex3f(points[j].x,points[j].y,points[j].z);
				glVertex3f(points[j].x,points[j].y,-points[j].z);
				glVertex3f(points[j+1].x,points[j+1].y,-points[j+1].z);
				glVertex3f(points[j+1].x,points[j+1].y,points[j+1].z);

                //lower hemisphere



			}glEnd();
		}
        glColor3f(255, 255, 255);
        double spike_height = height/4;

        glBegin(GL_QUADS);{
        //upper hemisphere
            glVertex3f(radius,0,-spike_height);
            glVertex3f(-radius,0,-spike_height);
            glVertex3f(-radius,0,spike_height);
            glVertex3f(radius,0,spike_height);

            //lower hemisphere



        }glEnd();

        glBegin(GL_QUADS);{
        //upper hemisphere
            glVertex3f(0,radius,-spike_height);
            glVertex3f(0,-radius,-spike_height);
            glVertex3f(0,-radius,spike_height);
            glVertex3f(0,radius,spike_height);

            //lower hemisphere



        }glEnd();

}




void keyboardListener(unsigned char key, int x,int y){
	switch(key){

        case 'w':
            //tilt counter_clockwise
            pos_wheel.y+=UNIT_MOVE*cos(radian(wheel_theta));
            pos_wheel.z+=UNIT_MOVE*sin(radian(wheel_theta));
            rotate_degree -= (360/(2*pi*radius))*UNIT_MOVE;
            rotate_degree=rotate_degree%360;
			break;
        case 's':
            pos_wheel.y-=UNIT_MOVE*cos(radian(wheel_theta));
            pos_wheel.z-=UNIT_MOVE*sin(radian(wheel_theta));
            rotate_degree += (360/(2*pi*radius))*UNIT_MOVE;
            rotate_degree=rotate_degree%360;
			break;
        case 'd':
            //tilt counter_clockwise
            wheel_theta+=UNIT_ROTATE;
            wheel_theta=wheel_theta%360;
            //cout<<wheel_theta<<endl;
			break;
        case 'a':
            //tilt counter_clockwise
            wheel_theta-=UNIT_ROTATE;
            wheel_theta=wheel_theta%360;
			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void draw_wheel()
{


    glTranslatef(0, 0, radius);
    glRotatef(90, 0, 1, 0);
    //WHEEL PLACED

    //MOVEMENT
    glTranslatef(pos_wheel.x,pos_wheel.y,pos_wheel.z);

    //glTranslatef(wheel_x*sin(radian(wheel_theta)),wheel_x*cos(radian(wheel_theta)), 0);
    glRotatef(wheel_theta, 1, 0, 0);
    glRotatef(rotate_degree, 0, 0, 1);
    //cout<<wheel_x<<endl;




    drawCylinder(30, 500);


}


void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(0,0,20,	0,0,0,	0,1,0);
	//gluLookAt(pos.x, pos.y, pos.z, pos.x+l.x, pos.y+l.y, pos.z+l.z, u.x, u.y, u.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);

    //drawSS();
    //drawCone(10, 5, 10);


    //glRotatef(wheel_x, 0, 1, 0);
    //glTranslatef(wheel_x,0,0);
    draw_wheel();



    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;


	wheel_theta = 0;
	pos_wheel.x = 0;
	pos_wheel.y = 0;
	pos_wheel.z = 0;
	radius = 30;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(50,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
    srand (time(NULL));
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");


	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
