#include<stdio.h>
#include<iostream>
#include <windows.h>
#include <GL/glut.h>

#include "bitmap_image.hpp"

#define pi (2*acos(0.0))
#define UNIT_MOVE 2
#define UNIT_ROTATE 3
#include "1705030_Header.h"
#include<stdlib.h>
string filename_scenefile;
string filename_imagefile;
string filename_mybuffer;
int image_count;
#include <map>

#define DIRECTORY "S:\\STUDY\\CSE 410\\ofl9 3\\1705030\\"


using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;


extern Vector3D pos;
Vector3D u, r, l;

extern vector<Light*> lights;
extern vector<Object*> objects;
extern Vector3D pos;
extern int level_of_recursion;


int i_Height, i_Width, no_of_objects, no_of_light_source;


int w_Height = 500;
int w_Width = 500;
int field_of_view=80;

void release_memory()
{
    for(int j=0; j<lights.size(); j++) delete lights[j];
    lights.clear();
    for(int i=0; i<objects.size(); i++) delete objects[i];
    objects.clear();
}


void initialize_points(double pos_x, double pos_y, double pos_z, double u_x, double u_y, double u_z, double r_x, double r_y, double r_z, double l_x, double l_y, double l_z)
{
    pos.x=pos_x;pos.y=pos_y;pos.z=pos_z;    u.x=u_x;u.y=u_y;u.z=u_z;    r.x=r_x;r.y=r_y;r.z=r_z;    l.x=l_x;l.y=l_y;l.z=l_z;
    image_count=11;
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




void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct Vector3D points[100];
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
	struct Vector3D points[100][100];
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


void capture();
void keyboardListener(unsigned char key, int x,int y){
    Vector3D cross_u, cross_r, cross_l;
	switch(key){
        case '0':
            capture();
            break;

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
			pos = sub_point(pos, u, 1, UNIT_MOVE);
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


void mouseListener(int button, int state, int x, int y){
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){
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

	for(int i=0; i<objects.size(); i++) objects[i]->draw();
	for(int i=0; i<lights.size(); i++) lights[i]->draw();


	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void set_dim(int dim)
{
    i_Height=dim;
    i_Width=dim;
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
void initialize_file()
{
    filename_imagefile = "out.bmp";
    filename_scenefile = "scene.txt";
    filename_mybuffer = "my_buffer.txt";
    //filename_scenefile = "S:\\STUDY\\CSE 410\\ofl9 3\\help_2\\my_code\\input.txt";
    remove(filename_imagefile.c_str());
    remove(filename_mybuffer.c_str());
}



void loadData()
{
    ifstream scene_file;
    scene_file.open(DIRECTORY+filename_scenefile);
    if(scene_file.is_open()==true)
    {
        double tmp_x, tmp_y, tmp_z, tmp_w, rad;
        double tmp_x2, tmp_y2, tmp_z2;
        double tmp_x1, tmp_y1, tmp_z1;
        double A,B,C,D,E,F,G,H,I,J;
        string input_msg;
        scene_file>>level_of_recursion;
        int temp_dim;
        scene_file>>temp_dim;
        set_dim(temp_dim);
        scene_file>>no_of_objects;
        int tmp_cnt = no_of_objects;
        while(tmp_cnt!=0)
        {
            scene_file>>input_msg;
            if(input_msg=="sphere")
            {
                Object *sphere;
                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                scene_file>>rad;
                sphere = new Sphere(tmp_x, tmp_y, tmp_z, rad);
                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                sphere->set_color(tmp_x, tmp_y, tmp_z);
                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                scene_file>>tmp_w;
                sphere->setCoEfficients(tmp_x, tmp_y, tmp_z, tmp_w);
                scene_file>>tmp_x;
                sphere->set_shine(tmp_x);

                objects.push_back(sphere);
                //cout<<"in load"<<endl;
                tmp_cnt--;



            }
            else if(input_msg=="triangle")
            {
                Object *triangle;
                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;

                scene_file>>tmp_x1;
                scene_file>>tmp_y1;
                scene_file>>tmp_z1;

                scene_file>>tmp_x2;
                scene_file>>tmp_y2;
                scene_file>>tmp_z2;

                triangle = new Triangle(tmp_x, tmp_y, tmp_z, tmp_x1, tmp_y1, tmp_z1, tmp_x2, tmp_y2, tmp_z2);

                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                triangle->set_color(tmp_x, tmp_y, tmp_z);

                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                scene_file>>tmp_w;
                triangle->setCoEfficients(tmp_x, tmp_y, tmp_z, tmp_w);
                scene_file>>tmp_x;
                triangle->set_shine(tmp_x);
                objects.push_back(triangle);
                //cout<<"in load tri"<<endl;
                tmp_cnt--;



            }
            else if(input_msg=="general")
            {
                Object *general;
                double params[16];
                scene_file>>params[0]>>params[1]>>params[2]>>params[3]>>params[4]>>params[5]>>params[6]>>params[7]>>params[8]>>params[9];
                scene_file>>params[10];
                scene_file>>params[11];
                scene_file>>params[12];

                scene_file>>params[13];
                scene_file>>params[14];
                scene_file>>params[15];
                general = new GeneralQuadratic(params);
                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                general->set_color(tmp_x, tmp_y, tmp_z);

                scene_file>>tmp_x;
                scene_file>>tmp_y;
                scene_file>>tmp_z;
                scene_file>>tmp_w;
                general->setCoEfficients(tmp_x, tmp_y, tmp_z, tmp_w);
                scene_file>>tmp_x;
                general->set_shine(tmp_x);
                objects.push_back(general);
                //cout<<"in load gw"<<endl;
                tmp_cnt--;



            }
        }
        scene_file>>no_of_light_source;
        tmp_cnt = no_of_light_source;

        while(tmp_cnt!=0){

            Light *point_light;
            scene_file>>tmp_x;
            scene_file>>tmp_y;
            scene_file>>tmp_z;

            point_light=new Light();
            point_light->set_type("point");
            point_light->light_pos.x = tmp_x;
            point_light->light_pos.y = tmp_y;
            point_light->light_pos.z = tmp_z;

            scene_file>>tmp_x;
            scene_file>>tmp_y;
            scene_file>>tmp_z;
            point_light->set_color(tmp_x,tmp_y,tmp_z);
            lights.push_back(point_light);
            tmp_cnt--;
        }

        scene_file>>no_of_light_source;
        tmp_cnt = no_of_light_source;
        //tmp_cnt = 0;

        while(tmp_cnt!=0){

            Light *spot_light;
            scene_file>>tmp_x;
            scene_file>>tmp_y;
            scene_file>>tmp_z;

            spot_light=new Light();
            spot_light->set_type("spot");
            spot_light->light_pos.x = tmp_x;
            spot_light->light_pos.y = tmp_y;
            spot_light->light_pos.z = tmp_z;

            scene_file>>tmp_x;
            scene_file>>tmp_y;
            scene_file>>tmp_z;
            spot_light->set_color(tmp_x,tmp_y,tmp_z);

            scene_file>>tmp_x;
            scene_file>>tmp_y;
            scene_file>>tmp_z;
            spot_light->set_direction(tmp_x,tmp_y,tmp_z);
            scene_file>>tmp_x;
            spot_light->set_angle(tmp_x);

            lights.push_back(spot_light);
            tmp_cnt--;
            //cout<<"in spot lite"<<endl;
            //cout<<spot_light->type<<endl;
        }

        Object *floor;
        floor=new Floor(1000,20);
        floor->setCoEfficients(0.45,0.45,0.45,0.45);

        floor->set_shine(15);

        objects.push_back(floor);

    }
    else
    {
        cout<<"File not found!"<<endl;
    }



}
void set_black_backgroud(bitmap_image image, int width, int height)
{
    for(int i=0;i<width;i++){
        for(int j=0;j<height;j++){
            image.set_pixel(i,j,0,0,0);
        }
    }

}

void capture()
{
    cout<<"Capturing the image..."<<endl;
    filename_imagefile =  "Output_"+to_string(image_count)+".bmp";
    image_count++;
    string image_dir=DIRECTORY + filename_imagefile;
    double du=w_Width/(i_Width*1.0);
    double dv=w_Height/(i_Height*1.0);

    bitmap_image image(i_Width,i_Height);
    set_black_backgroud(image, i_Width, i_Height);
    image.save_image(image_dir);


    double planeDistance=(w_Height/2.0)/tan(radian(field_of_view)/2.0);

    struct Vector3D topleft;
    topleft.x = pos.x + l.x * planeDistance - r.x * (w_Width/2.0) + u.x * (w_Height/2.0) + (r.x*0.5*du)-(u.x*0.5*dv);
    topleft.y = pos.y + l.y * planeDistance - r.y * (w_Width/2.0) + u.y * (w_Height/2.0) + (r.y*0.5*du)-(u.y*0.5*dv);
    topleft.z = pos.z + l.z * planeDistance - r.z * (w_Width/2.0) + u.z * (w_Height/2.0) + (r.z*0.5*du)-(u.z*0.5*dv);


    int next_obj;
    bool next_found = false;
    double tnew;


    for(int i=0;i<i_Width;i++){
        for(int j=0;j<i_Height;j++){
            struct Vector3D curPixel = add_point(topleft, sub_point(scalar_multiplication(r, i*du), scalar_multiplication(u, j*dv)));

            Ray newRay;
            newRay.ray_dir = normalize(sub_point(curPixel, pos));;
            newRay.ray_pos = pos;

            double *color=new double[3];
            color[0]=0.0;
            color[1]=0.0;
            color[2]=0.0;
            double tcur=1000000;

            for(int p=0;p<objects.size();p++){
                tnew=objects[p]->intersect(&newRay,new double[3],0);
                if(tcur>tnew && tnew>0){
                    tcur=tnew;
                    next_obj=p;
                    next_found = true;
                }
            }
            if(next_found==false){

                delete[] color;

            }
            else
            {
                tcur=objects[next_obj]->intersect(&newRay,color,1);

                color[0]=color[0]*255;
                color[1]=color[1]*255;
                color[2]=color[2]*255;
                image.set_pixel(i,j,color[0],color[1],color[2]);
                delete[] color;

            }

        }

    }
    image.save_image(image_dir);;
    cout<<"Image Saved!"<<endl;
}

int main(int argc, char **argv){
    initialize_file();
    loadData();
    atexit(release_memory);
	glutInit(&argc,argv);
	glutInitWindowSize(w_Width, w_Height);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	glutCreateWindow("My OpenGL Program");

	initialize_points(150, 130, 55, 0, 0, 1, (-1.0)/sqrt(2.0), (1.0)/sqrt(2.0), 0, (-1.0)/sqrt(2.0), (-1.0)/sqrt(2.0), 0);

	init();

	glEnable(GL_DEPTH_TEST);

	glutDisplayFunc(display);
	glutIdleFunc(animate);

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();

	return 0;
}
