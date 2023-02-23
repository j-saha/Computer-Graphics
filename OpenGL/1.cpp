#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<iostream>
#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define UNIT_MOVE 2
#define UNIT_ROTATE 3

using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;
};

point pos;
point u, r, l;
int cylinder_height;
int global_radius;

point add_point(point a, point b, int m=1, int n=1)
{
    point c;
    c.x = m*a.x+n*b.x; c.y=m*a.y+n*b.y; c.z=m*a.z+n*b.z;
    return c;
}
point sub_point(point a, point b, int m=1, int n=1)
{
    point c;
    c.x = m*a.x-n*b.x; c.y=m*a.y-n*b.y; c.z=m*a.z-n*b.z;
    return c;
}
point cross_product(point a, point b)
{
    point c;
    c.x = a.y*b.z - a.z*b.y;
    c.y = a.z*b.x - a.x*b.z;
    c.z = a.x*b.y - a.y*b.x;
    return c;
}

point scalar_multiplication(point p, double n)
{
    point c;
    c.x = n*p.x;
    c.y = n*p.y;
    c.z = n*p.z;
    return c;
}



double radian(double degree)
{
    return degree*pi/180;
}


void initialize_points(double pos_x, double pos_y, double pos_z, double u_x, double u_y, double u_z, double r_x, double r_y, double r_z, double l_x, double l_y, double l_z)
{
    pos.x=pos_x;pos.y=pos_y;pos.z=pos_z;    u.x=u_x;u.y=u_y;u.z=u_z;    r.x=r_x;r.y=r_y;r.z=r_z;    l.x=l_x;l.y=l_y;l.z=l_z;
    global_radius = 10;
    cylinder_height = 36;
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

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}


void drawCircle(double radius,int segments, int z=0)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,z);
			glVertex3f(points[i+1].x,points[i+1].y,z);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void draw8thSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*pi/2);
			points[i][j].y=r*sin(((double)j/(double)slices)*pi/2);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        //glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}


void draw4thCylinder(double radius,int stacks)
{
	struct point points[1000];

	//generate points
	for(int i=0;i<=stacks;i++)
	{

			points[i].x=radius*cos(((double)i/(double)stacks)*pi/2);
			points[i].y=radius*sin(((double)i/(double)stacks)*pi/2);
			points[i].z=cylinder_height/2;
			//cout<<points[i].x;

	}
	//draw quads using generated points


		for(int j=0;j<stacks;j++)
		{
		            //glColor3f((double)1/(double)stacks,(double)1/(double)stacks,(double)1/(double)stacks);

			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[j].x,points[j].y,points[j].z);
				glVertex3f(points[j].x,points[j].y,-points[j].z);
				glVertex3f(points[j+1].x,points[j+1].y,-points[j+1].z);
				glVertex3f(points[j+1].x,points[j+1].y,points[j+1].z);

			}glEnd();
		}
		//drawCircle(radius, 50, cylinder_height);


}

void drawCylinder(double radius,int stacks)
{
	struct point points[1000];



	//generate points
	for(int i=0;i<=stacks;i++)
	{

			points[i].x=radius*cos(((double)i/(double)stacks)*2*pi);
			points[i].y=radius*sin(((double)i/(double)stacks)*2*pi);
			points[i].z=cylinder_height/2;
			cout<<points[i].x;

	}
	//draw quads using generated points


		for(int j=0;j<stacks;j++)
		{
		            //glColor3f((double)1/(double)stacks,(double)1/(double)stacks,(double)1/(double)stacks);

			glBegin(GL_QUADS);{
				glVertex3f(points[j].x,points[j].y,points[j].z);
				glVertex3f(points[j].x,points[j].y,-points[j].z);
				glVertex3f(points[j+1].x,points[j+1].y,-points[j+1].z);
				glVertex3f(points[j+1].x,points[j+1].y,points[j+1].z);

			}glEnd();
		}
		//drawCircle(radius, 50, cylinder_height);


}



void place_1st_sphere(int radius)
{
    glPushMatrix();

    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);

    draw8thSphere(global_radius, 90, 90);


    glPopMatrix();


}

void place_upper_sphere(int angle, int radius)
{
    glPushMatrix();
    glRotatef(angle,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);
    draw8thSphere(global_radius, 90, 90);
    glPopMatrix();

}

void place_upperZ_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(angle,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 90);
    glPopMatrix();

}
void place_upperX_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(angle,1,0,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 90);
    glPopMatrix();

}
void place_upperY_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(angle,0,1,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 900);
    glPopMatrix();

}

void place_backZ_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(angle,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 900);
    glPopMatrix();

}
void place_backX_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(angle,1,0,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 900);
    glPopMatrix();

}
void place_backY_cylinder(int angle, int radius)
{
    glPushMatrix();
    glRotatef(180,0,0,1);
    glRotatef(angle,0,1,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,0);
    draw4thCylinder(global_radius, 900);
    glPopMatrix();

}


void place_1st_lower_sphere(int angle, int radius)
{
    glPushMatrix();
    glRotatef(angle,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);


    //glRotatef(180,0,0,1);


    draw8thSphere(global_radius, 90, 90);

    glPopMatrix();


}

void my_sphere_draw(int radius)
{

    glColor3f(255,0,0);
    place_1st_sphere(10);
    place_upper_sphere(90, 10);
    place_upper_sphere(-90, 10);
    place_upper_sphere(180, 10);


    //LOWERRR

    glPushMatrix();
    glRotatef(180,1,0,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);
    draw8thSphere(global_radius, 50, 50);
    glPopMatrix();



    glPushMatrix();
    glRotatef(180,0,1,0);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);
    draw8thSphere(global_radius, 50, 50);
    glPopMatrix();


    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(90,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);
    draw8thSphere(global_radius, 50, 50);
    glPopMatrix();


    glPushMatrix();
    glRotatef(180,0,1,0);
    glRotatef(-90,0,0,1);
    glTranslatef(cylinder_height/2,cylinder_height/2,cylinder_height/2);
    draw8thSphere(global_radius, 50, 50);
    glPopMatrix();

}

void my_cylinder_draw()
{
    glColor3f(0,255,0);

    glPushMatrix();
    glTranslatef(cylinder_height/2,cylinder_height/2, 0);
    draw4thCylinder(global_radius, 90);
    glPopMatrix();

    place_upperZ_cylinder(90, 10);
    place_upperZ_cylinder(-90, 10);
    place_upperX_cylinder(90, 10);
    place_upperX_cylinder(-90, 10);
    place_upperY_cylinder(90, 10);
    place_upperY_cylinder(-90, 10);

    place_upperZ_cylinder(180, 10);

    place_backZ_cylinder(90, 10);
    place_backZ_cylinder(-90, 10);
    place_backX_cylinder(90, 10);
    place_backX_cylinder(-90, 10);
    place_backY_cylinder(90, 10);
    place_backY_cylinder(-90, 10);
}

void place_Xsquare(int angle)
{
    glPushMatrix();
    glColor3f(255,255,255);
    glRotatef(angle,1,0,0);
    glTranslatef(0,0, cylinder_height/2+global_radius);
    drawSquare(cylinder_height/2);
    glPopMatrix();

}
void place_Ysquare(int angle)
{
    glPushMatrix();
    glColor3f(255,255,255);
    glRotatef(angle,0,1,0);
    glTranslatef(0,0,cylinder_height/2+global_radius);
    drawSquare(cylinder_height/2);
    glPopMatrix();

}
void place_Zsquare(int sign)
{
    glPushMatrix();
    glColor3f(255,255,255);
    //glRotatef(angle,0,0,1);
    glTranslatef(0,0,sign*(cylinder_height/2+global_radius));
    drawSquare(cylinder_height/2);
    glPopMatrix();

}

void my_square_draw()
{
    place_Xsquare(90);
    place_Xsquare(-90);
    place_Ysquare(90);
    place_Ysquare(-90);
    place_Zsquare(1);
    place_Zsquare(-1);


}



void keyboardListener(unsigned char key, int x,int y){
    point cross_u, cross_r, cross_l;
	switch(key){

		case '1':
			//rotate left
			cross_r = cross_product(u, r);
			cross_l = cross_product(u, l);

			r = add_point(scalar_multiplication(r, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_r, sin(radian(UNIT_ROTATE))));
			l = add_point(scalar_multiplication(l, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_l, sin(radian(UNIT_ROTATE))));


			break;
        case '2':
			//rotate right
			cross_r = cross_product(u, r);
			cross_l = cross_product(u, l);

			r = add_point(scalar_multiplication(r, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_r, sin(-radian(UNIT_ROTATE))));
			l = add_point(scalar_multiplication(l, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_l, sin(-radian(UNIT_ROTATE))));


			break;
        case '3':
            //look up
            cross_u = cross_product(r, u);
			cross_l = cross_product(r, l);

			u = add_point(scalar_multiplication(u, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_u, sin(radian(UNIT_ROTATE))));
			l = add_point(scalar_multiplication(l, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_l, sin(radian(UNIT_ROTATE))));


			break;
        case '4':
            //look down
            cross_u = cross_product(r, u);
			cross_l = cross_product(r, l);

			u = add_point(scalar_multiplication(u, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_u, sin(-radian(UNIT_ROTATE))));
			l = add_point(scalar_multiplication(l, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_l, sin(-radian(UNIT_ROTATE))));


			break;
        case '5':
            //tilt clockwise
			cross_u = cross_product(l, u);
			cross_r = cross_product(l, r);

			u = add_point(scalar_multiplication(u, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_u, sin(radian(UNIT_ROTATE))));
			r = add_point(scalar_multiplication(r, cos(radian(UNIT_ROTATE))) , scalar_multiplication(cross_r, sin(radian(UNIT_ROTATE))));


			break;
        case '6':
            //tilt counter_clockwise
            cross_u = cross_product(l, u);
			cross_r = cross_product(l, r);

			u = add_point(scalar_multiplication(u, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_u, sin(-radian(UNIT_ROTATE))));
			r = add_point(scalar_multiplication(r, cos(-radian(UNIT_ROTATE))) , scalar_multiplication(cross_r, sin(-radian(UNIT_ROTATE))));


			break;
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos = sub_point(pos, l, 1, UNIT_MOVE);
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos = add_point(pos, l, 1, UNIT_MOVE);
			break;

		case GLUT_KEY_RIGHT:
			pos = add_point(pos, r, 1, UNIT_MOVE);
			break;
		case GLUT_KEY_LEFT:
			pos = sub_point(pos, r, 1, UNIT_MOVE);
			break;

		case GLUT_KEY_PAGE_UP:
		    pos = add_point(pos, u, 1, UNIT_MOVE);
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;
			pos = sub_point(pos, u, 1, UNIT_MOVE);

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
            if(cylinder_height-4>=0)
            {
                cylinder_height-=4;
                global_radius+=2;
            }

			break;
		case GLUT_KEY_END:
		    if(global_radius-2>=0)
            {
                cylinder_height+=4;
                global_radius-=2;
            }

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
	//gluLookAt(0,0,20,	0,0,0,	0,1,0);
	gluLookAt(pos.x, pos.y, pos.z, pos.x+l.x, pos.y+l.y, pos.z+l.z, u.x, u.y, u.z);


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

    my_sphere_draw(10);
    my_cylinder_draw();
    my_square_draw();
    //draw4thCylinder(10, 100);

    //drawCylinder(30, 500);

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

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
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	initialize_points(100, 100, 0, 0, 0, 1, (-1.0)/sqrt(2.0), (1.0)/sqrt(2.0), 0, (-1.0)/sqrt(2.0), (-1.0)/sqrt(2.0), 0);

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
