#include <windows.h>
#include<bits/stdc++.h>
#include<stdio.h>
#include<iostream>
#include "bitmap_image.hpp"
#define pi (2*acos(0.0))
#include <GL/glut.h>
#include <algorithm>

#define DELTA 0.00001
#include <map>
using namespace std;

struct Vector3D
{
	double x,y,z;
};
Vector3D add_point(Vector3D a, Vector3D b, double m=1.0, double n=1.0)
{
    Vector3D c;
    c.x = m*a.x+n*b.x; c.y=m*a.y+n*b.y; c.z=m*a.z+n*b.z;
    return c;
}
Vector3D sub_point(Vector3D a, Vector3D b, double m=1.0, double n=1.0)
{
    Vector3D c;
    c.x = m*a.x-n*b.x; c.y=m*a.y-n*b.y; c.z=m*a.z-n*b.z;
    return c;
}
Vector3D cross_product(Vector3D a, Vector3D b)
{
    Vector3D c;
    c.x = a.y*b.z - a.z*b.y;
    c.y = a.z*b.x - a.x*b.z;
    c.z = a.x*b.y - a.y*b.x;
    return c;
}
double dot_product(Vector3D a, Vector3D b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

Vector3D scalar_multiplication(Vector3D p, double n)
{
    Vector3D c;
    c.x = n*p.x;
    c.y = n*p.y;
    c.z = n*p.z;
    return c;
}



double radian(double degree)
{
    return degree*pi/180;
}


//done
Vector3D normalize(Vector3D p)
{

    double d = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
    if(d==0) return p;

    Vector3D c;
    c.x = p.x/d;
    c.y = p.y/d;
    c.z = p.z/d;
    return c;


}


class Ray
{
public:
    struct Vector3D ray_dir;
    struct Vector3D ray_pos;
};


class Light
{
public:
    struct Vector3D light_pos;
    struct Vector3D spot_dir;
    double color[3];
    double angle;
    string type;
    int size_light;

    void set_direction(double x, double y, double z)
    {
        spot_dir.x = x;
        spot_dir.y = y;
        spot_dir.z = z;
    }
    void set_angle(double a)
    {
        angle = a;
    }
    void set_type(string t)
    {
        type=t;
        if(t=="point") size_light = 5;
        else size_light = 7;
    }
    void draw()
    {
        glColor3f(color[0],color[1],color[2]);
        glPointSize(size_light);

        glBegin(GL_POINTS);
        {
            glVertex3f(light_pos.x,light_pos.y,light_pos.z);
        }
        glEnd();
    }
    void set_color(double r, double g, double b)
    {
        color[0]=r;
        color[1]=g;
        color[2]=b;
    }
};

vector<Light*> lights;
double clamp_rgb_value(double value)
{
    if(value<0) return 0;
    if(value>1) return 1;
    return value;
}

class Object
{
public:
    Vector3D reference_point;
    double height, width, length;
    double color[3];
    double coEfficients[4];
    int shine;
    Object(){}
    void set_color(double r,double g,double b)
    {
        color[0]=r;
        color[1]=g;
        color[2]=b;
    }

    void setCoEfficients(double c1,double c2,double c3,double c4)
    {
        coEfficients[0]=c1;
        coEfficients[1]=c2;
        coEfficients[2]=c3;
        coEfficients[3]=c4;
    }


    virtual double get_T_value(Ray *r)
    {
        return -1;
	}
	virtual double intersect(Ray *r, double *obj_color, int level){
        return -1;
	}
	virtual void draw(){}
    void set_shine(int shine)
    {
        this->shine=shine;
    }

};


vector<Object*> objects;
Vector3D pos;
int level_of_recursion;

    double run_recursion(Vector3D normal, Vector3D intersection, double *obj_color, Ray *r, int level, double t, Object *object)
    {
        if(level<level_of_recursion){

            Ray reflect_ray;
            reflect_ray.ray_dir = normalize(sub_point(r->ray_dir, scalar_multiplication(normal, dot_product(r->ray_dir, normal)*2.0)));
            reflect_ray.ray_pos = add_point(intersection, reflect_ray.ray_dir, 1.0, DELTA);

            int next_obj;
            bool next_found = false;
            double tcur=100000;

            for(int p=0; p<objects.size(); p++){
                double tnew=objects[p]->intersect(&reflect_ray,new double[3],0);

                if(tcur>tnew && tnew>0){
                    tcur=tnew;
                    next_obj=p;
                    next_found = true;
                }
            }

            if(next_found==false){

                obj_color[0] = clamp_rgb_value(obj_color[0]);
                obj_color[1] = clamp_rgb_value(obj_color[1]);
                obj_color[2] = clamp_rgb_value(obj_color[2]);
                return tcur;
            }
            else
            {
                double *reflect_color=new double[3];
                 tcur = objects[next_obj]->intersect(&reflect_ray,reflect_color,level+1);
                 if(tcur!=-1){
                    obj_color[0]+=reflect_color[0]*object->coEfficients[3];
                    obj_color[1]+=reflect_color[1]*object->coEfficients[3];
                    obj_color[2]+=reflect_color[2]*object->coEfficients[3];
                }
                obj_color[0] = clamp_rgb_value(obj_color[0]);
                obj_color[1] = clamp_rgb_value(obj_color[1]);
                obj_color[2] = clamp_rgb_value(obj_color[2]);
                delete[] reflect_color;
                return tcur;
            }
        }
        return t;
    }

    void check_lights(Ray *r, Vector3D intersection, Vector3D normal, double *obj_color, Object *object)
    {
        for(int i=0;i<lights.size();i++){


            Ray light_ray;
            light_ray.ray_dir = normalize(sub_point(lights[i]->light_pos, intersection));
            light_ray.ray_pos = add_point(intersection, light_ray.ray_dir, 1, DELTA);

            if(lights[i]->type=="spot")
            {
                Vector3D spot_direction = normalize(lights[i]->spot_dir);
                double light_angle = std::acos(dot_product(scalar_multiplication(light_ray.ray_dir, -1.0), spot_direction));
                light_angle = light_angle*(180.0/pi);
                if(light_angle>lights[i]->angle) continue;
                //else cout<<"In range of spotlight"<<endl;
            }

            int is_contine=0;
            double light_length =sqrt((lights[i]->light_pos.x - intersection.x)*(lights[i]->light_pos.x - intersection.x)+(lights[i]->light_pos.y - intersection.y)*(lights[i]->light_pos.y - intersection.y)+(lights[i]->light_pos.z - intersection.z)*(lights[i]->light_pos.z - intersection.z));
            for(int j=0;j<objects.size();j++){
                double tr=objects[j]->get_T_value(&light_ray);

                if(light_length>tr && tr>0){
                    is_contine=1;
                }
                if(is_contine==1)
                {
                    break;
                }
            }
            //light will reach the obj if it's not obscure by others
            if(is_contine==1) continue;

            Vector3D reflected_ray = normalize(sub_point(light_ray.ray_dir, scalar_multiplication(normal, dot_product(light_ray.ray_dir, normal)*2.0)));
            double lambert=dot_product(light_ray.ray_dir,normal);
            double phong=dot_product(reflected_ray,r->ray_dir);

            if(lambert < 0) lambert = 0;
            if(phong < 0) phong = 0;

            obj_color[0]+=(object->color[0]*lights[i]->color[0]*lambert*object->coEfficients[1])+(lights[i]->color[0]*object->coEfficients[2]*pow(phong,object->shine)*object->color[0]);
            obj_color[1]+=(object->color[1]*lights[i]->color[1]*lambert*object->coEfficients[1])+(lights[i]->color[1]*object->coEfficients[2]*pow(phong,object->shine)*object->color[1]);
            obj_color[2]+=(object->color[2]*lights[i]->color[2]*lambert*object->coEfficients[1])+(lights[i]->color[2]*object->coEfficients[2]*pow(phong,object->shine)*object->color[2]);



        }
    }
class Sphere:public Object
{
    int stacks, slices;
    double sphere_radius;
public:
    Sphere(double x, double y, double z,double radius){
        reference_point.x=x;
        reference_point.y=y;
        reference_point.z=z;
        sphere_radius = radius;
        stacks = 35;
        slices = 35;
    }

    //done
    void draw()
    {
        struct Vector3D points[100][100];
        int i,j;
        double h,r;
        //generate points
        for(i=0;i<=stacks;i++)
        {
            h=sphere_radius*sin(((double)i/(double)stacks)*(pi/2.0));
            r=sphere_radius*cos(((double)i/(double)stacks)*(pi/2.0));
            for(j=0;j<=slices;j++)
            {
                points[i][j].x=r*cos(((double)j/(double)slices)*2.0*pi);
                points[i][j].y=r*sin(((double)j/(double)slices)*2.0*pi);
                points[i][j].z=h;
            }
        }
            double cr=color[0];
            double cg=color[1];
            double cb=color[2];
        //draw quads using generated points
        for(i=0;i<stacks;i++)
        {

            glColor3f(cr,cg,cb);
            for(j=0;j<slices;j++)
            {
                glBegin(GL_QUADS);{
                    Vector3D point_1 = add_point(points[i][j], reference_point);
                    Vector3D point_2 = add_point(points[i][j+1], reference_point);
                    Vector3D point_3 = add_point(points[i+1][j+1], reference_point);
                    Vector3D point_4 = add_point(points[i+1][j], reference_point);

                    Vector3D point_5 = add_point(scalar_multiplication(points[i][j],-1.0), reference_point);
                    Vector3D point_6 = add_point(scalar_multiplication(points[i][j+1],-1.0), reference_point);
                    Vector3D point_7 = add_point(scalar_multiplication(points[i+1][j+1],-1.0), reference_point);
                    Vector3D point_8 = add_point(scalar_multiplication(points[i+1][j],-1.0), reference_point);
                    //upper hemisphere
                    glVertex3f(point_1.x, point_1.y, point_1.z);
                    glVertex3f(point_2.x, point_2.y, point_2.z);
                    glVertex3f(point_3.x, point_3.y, point_3.z);
                    glVertex3f(point_4.x, point_4.y, point_4.z);

                    glVertex3f(point_5.x, point_5.y, point_5.z);
                    glVertex3f(point_6.x, point_6.y, point_6.z);
                    glVertex3f(point_7.x, point_7.y, point_7.z);
                    glVertex3f(point_8.x, point_8.y, point_8.z);


                }glEnd();
            }
        }
    }
    double getMinPositive(double a, double b)
    {
        if(a>b)
        {
            if(b>0) return b;
            else{
                if(a>0) return a;
                else return -1;
            }
        }
        else
        {
            if(a>0) return a;
            else{
                if(b>0) return b;
                else return -1;
            }
        }

    }
    double get_T_value(Ray *r)
    {
        double a=dot_product(r->ray_dir,r->ray_dir);

        double b=2.0*(dot_product(r->ray_dir,sub_point(r->ray_pos, reference_point)));

        double c=dot_product(sub_point(r->ray_pos, reference_point),sub_point(r->ray_pos, reference_point))-sphere_radius*sphere_radius;

        double d=pow(b,2)-(4*a*c);
        if(d>=0){
            d = sqrt(d);
            double ta=(-b+d)/(a*2.0);
            double tb=(-b-d)/(a*2.0);
            return getMinPositive(ta, tb);

        }
        return -1;


    }

    double intersect(Ray *r, double *obj_color, int level){

        double t = get_T_value(r);

        if(t<=0) return -1;
        if(level==0) return t;

        struct Vector3D intersection = add_point(r->ray_pos,r->ray_dir,1,t);

        obj_color[0]=this->color[0]*this->coEfficients[0];
        obj_color[1]=this->color[1]*this->coEfficients[0];
        obj_color[2]=this->color[2]*this->coEfficients[0];
        struct Vector3D normal=normalize(sub_point(intersection, reference_point));

        //camera inside sphere
        double diff = sqrt((pos.x-reference_point.x)*(pos.x-reference_point.x)+(pos.y-reference_point.y)*(pos.y-reference_point.y)+(pos.z-reference_point.z)*(pos.z-reference_point.z));
        if(diff < sphere_radius)
        {
            normal = scalar_multiplication(normal, -1);
        }

        check_lights(r, intersection, normal, obj_color, this);
        return run_recursion(normal, intersection, obj_color, r, level, t, this);

    }

};


class Triangle:public Object
{
public:
    struct Vector3D p1;
    struct Vector3D p2;
    struct Vector3D p3;
    Triangle(double x1,double y1,double z1,double x2,double y2,double z2,double x3,double y3,double z3){
        this->p1.x=x1;
        this->p1.y=y1;
        this->p1.z=z1;
        this->p2.x=x2;
        this->p2.y=y2;
        this->p2.z=z2;
        this->p3.x=x3;
        this->p3.y=y3;
        this->p3.z=z3;
    }
    void draw()
    {
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(p1.x,p1.y,p1.z);
			glVertex3f(p2.x,p2.y,p2.z);
			glVertex3f(p3.x,p3.y,p3.z);
        }
        glEnd();
    }
    double get_T_value(Ray *r)
    {
        double Rox = r->ray_pos.x;
        double Roy = r->ray_pos.y;
        double Roz = r->ray_pos.z;

        double Rdx = r->ray_dir.x;
        double Rdy = r->ray_dir.y;
        double Rdz = r->ray_dir.z;

        double axRox = p1.x-Rox;
        double ayRoy = p1.y-Roy;
        double azRoz = p1.z-Roz;

        double axcx = p1.x-p3.x;
        double aycy = p1.y-p3.y;
        double azcz = p1.z-p3.z;

        double axbx = p1.x-p2.x;
        double ayby = p1.y-p2.y;
        double azbz = p1.z-p2.z;



        double A = axbx*(aycy*Rdz - azcz*Rdy) - axcx*(ayby*Rdz - azbz*Rdy) + Rdx*(ayby*azcz - azbz*aycy);
        if(A==0) return -1;

        double beta = (axRox*(aycy*Rdz - azcz*Rdy) - axcx*(ayRoy*Rdz - azRoz*Rdy) + Rdx*(ayRoy*azcz - azRoz*aycy))/A;
        double t = (axbx*(aycy*azRoz - azcz*ayRoy) - axcx*(ayby*azRoz - azbz*ayRoy) + axRox*(ayby*azcz - azbz*aycy))/A;
        double gamma = (axbx*(ayRoy*Rdz - azRoz*Rdy) - axRox*(ayby*Rdz - azbz*Rdy) + Rdx*(ayby*azRoz - azbz*ayRoy))/A;

        if(beta>0 && gamma>0 && t>0 && beta+gamma < 1)
            return t;
        return -1;

    }
    double intersect(Ray *r, double *obj_color, int level){
        double t=get_T_value(r);
        if(t<=0) return -1;
        if(level==0) return t;

        struct Vector3D intersection = add_point(r->ray_pos,r->ray_dir,1,t);

        obj_color[0]=this->color[0]*this->coEfficients[0];
        obj_color[1]=this->color[1]*this->coEfficients[0];
        obj_color[2]=this->color[2]*this->coEfficients[0];

        struct Vector3D normal = normalize(cross_product(sub_point(p2, p1), sub_point(p3, p1)));
        if(dot_product(scalar_multiplication(r->ray_dir, -1.0), normal)<0) normal = scalar_multiplication(normal, -1.0);


        check_lights(r, intersection, normal, obj_color, this);

        return run_recursion(normal, intersection, obj_color, r, level, t, this);




    }
};


class Floor:public Object
{
    double floorWidth, tileWidth;
public:


    Floor(double floorWidth,double tileWidth){
        this->tileWidth=tileWidth;
        this->floorWidth=floorWidth;
        reference_point.x=-floorWidth/2.0;
        reference_point.y=reference_point.x;
        reference_point.z=0;
    }
    void draw(){
        int color_floor = 1;
        int row = floorWidth/tileWidth;
        int colm = row;
        for(int i = 0; i<row; i++)
        {
            for(int j = 0; j<colm; j++)
            {

                glColor3f(color_floor, color_floor, color_floor);
                glBegin(GL_QUADS);
                {
                    glVertex3f(-floorWidth/2.0+tileWidth*i, -floorWidth/2.0+tileWidth*j, 0);
                    glVertex3f(-floorWidth/2.0+tileWidth*i, -floorWidth/2.0+tileWidth*j + tileWidth, 0);
                    glVertex3f(-floorWidth/2.0+tileWidth*i + tileWidth, -floorWidth/2.0+tileWidth*j + tileWidth, 0);
                    glVertex3f(-floorWidth/2.0+tileWidth*i + tileWidth, -floorWidth/2.0+tileWidth*j, 0);
                }
                glEnd();
                color_floor =  1 - color_floor;

            }

            color_floor = 1 - color_floor;
        }
    }

    bool in_between(Vector3D inter, Vector3D refer)
    {
        if(inter.x >= refer.x && inter.x <= -refer.x)
        {
            if(inter.y >= refer.y && inter.y <= -refer.y)
            {
                return true;
            }
        }
        return false;

    }

    void set_color_floor(Vector3D inter, Vector3D refer)
    {

        int color_flag = 1-((int)((inter.x - refer.x) / tileWidth)+(int)((inter.y - refer.y) / tileWidth))%2; //nth row + mth colm % 2
        color[0]=color_flag;
        color[1]=color_flag;
        color[2]=color_flag;

    }

    double get_T_value(Ray* r)
    {
        if(r->ray_dir.z==0) return -1;
        struct Vector3D normal;
        normal.x=0;
        normal.y=0;
        normal.z=1;
        if(dot_product(pos, normal)<0) normal = scalar_multiplication(normal, -1.0);
        return -dot_product(normal, r->ray_pos) / dot_product(normal, r->ray_dir);
    }

    double intersect(Ray *r, double *obj_color, int level){

        double t = get_T_value(r);
        if(t<=0) return -1;
        struct Vector3D intersection = add_point(r->ray_pos,r->ray_dir,1.0,t);

        if(!in_between(intersection, reference_point)) return -1;

        if(level==0) return t;

        set_color_floor(intersection, reference_point);


        obj_color[0]=this->color[0]*this->coEfficients[0];
        obj_color[1]=this->color[1]*this->coEfficients[0];
        obj_color[2]=this->color[2]*this->coEfficients[0];
        struct Vector3D normal;
        normal.x=0;
        normal.y=0;
        normal.z=1;
        if(dot_product(pos, normal)<0) normal = scalar_multiplication(normal, -1.0);

        check_lights(r, intersection, normal, obj_color, this);
        return run_recursion(normal, intersection, obj_color, r, level, t, this);
    }
};


class GeneralQuadratic:public Object
{
public:
    std::map<char, double> params;
    GeneralQuadratic(double params_arr[16]){

        params = {
            { 'A', params_arr[0] },
            { 'B', params_arr[1] },
            { 'C', params_arr[2] },
            { 'D', params_arr[3] },
            { 'E', params_arr[4] },
            { 'F', params_arr[5] },
            { 'G', params_arr[6] },
            { 'H', params_arr[7] },
            { 'I', params_arr[8] },
            { 'J', params_arr[9] }
        };

        this->reference_point.x=params_arr[10];
        this->reference_point.y=params_arr[11];
        this->reference_point.z=params_arr[12];
        this->length=params_arr[13];
        this->width=params_arr[14];
        this->height=params_arr[15];
    }

    bool check_root(Vector3D inter)
    {
        if(inter.x >= reference_point.x && inter.x <= reference_point.x + length || length==0)
        {
            if(inter.y >= reference_point.y && inter.y <= reference_point.y + width || width==0)
            {
                if(inter.z >= reference_point.z && inter.z <= reference_point.z + height || height==0)
                {
                    return true;

                }
                return false;
            }
            return false;
        }
        return false;
    }
    double get_t(bool root1_true, bool root2_true, double t1, double t2)
    {
        if(root1_true==true)
        {
            if(root2_true==true)
            {
                if(t1>t2) return t2;
                    return t1;
            }
            return t1;
        }
        else
        {
            if(root2_true==true) return t2;
            return -1;

        }

    }

    double get_T_value(Ray* r)
    {
        double a = params['A']*r->ray_dir.x*r->ray_dir.x+params['B']*r->ray_dir.y*r->ray_dir.y+params['C']*r->ray_dir.z*r->ray_dir.z+params['D']*r->ray_dir.x*r->ray_dir.y+params['E']*r->ray_dir.x*r->ray_dir.z+params['F']*r->ray_dir.y*r->ray_dir.z;
        double b = 2*params['A']*r->ray_pos.x*r->ray_dir.x+2*params['B']*r->ray_pos.y*r->ray_dir.y+2*params['C']*r->ray_pos.z*r->ray_dir.z+params['D']*(r->ray_pos.x*r->ray_dir.y+r->ray_pos.y*r->ray_dir.x)+params['E']*(r->ray_pos.x*r->ray_dir.z+r->ray_pos.z*r->ray_dir.x)+params['F']*(r->ray_pos.y*r->ray_dir.z+r->ray_dir.y*r->ray_pos.z)+params['G']*r->ray_dir.x+params['H']*r->ray_dir.y+params['I']*r->ray_dir.z;
        double c = params['A']*r->ray_pos.x*r->ray_pos.x+params['B']*r->ray_pos.y*r->ray_pos.y+params['C']*r->ray_pos.z*r->ray_pos.z+params['D']*r->ray_pos.x*r->ray_pos.y+params['E']*r->ray_pos.x*r->ray_pos.z+params['F']*r->ray_pos.y*r->ray_pos.z+params['G']*r->ray_pos.x+params['H']*r->ray_pos.y+params['I']*r->ray_pos.z+params['J'];

        if(a != 0){
            double d = b*b - 4*a*c;

            if(d>=0)
            {
                d = sqrt(d);
                double t1 = (-b - d)/(2*a);
                double t2 = (-b + d)/(2*a);

                Vector3D inter1 = add_point(r->ray_pos,r->ray_dir,1.0,t1);
                Vector3D inter2 = add_point(r->ray_pos,r->ray_dir,1.0,t2);

                bool root1_true = check_root(inter1);
                bool root2_true = check_root(inter2);

                return get_t(root1_true, root2_true, t1, t2);

            }
            return -1;
        }
        return -c/b;

    }

    double intersect(Ray *r, double *obj_color, int level){
        double t=get_T_value(r);
        if(t<=0) return -1;
        if(level==0) return t;
        struct Vector3D intersection = add_point(r->ray_pos,r->ray_dir,1.0,t);

        obj_color[0]=this->color[0]*this->coEfficients[0];
        obj_color[1]=this->color[1]*this->coEfficients[0];
        obj_color[2]=this->color[2]*this->coEfficients[0];

        Vector3D normal;
        normal.x = 2*params['A']*intersection.x+params['D']*intersection.y+params['E']*intersection.z+params['G'];
        normal.y = 2*params['B']*intersection.y+params['D']*intersection.x+params['F']*intersection.z+params['H'];
        normal.z = 2*params['C']*intersection.z+params['E']*intersection.x+params['F']*intersection.y+params['I'];

        normal = normalize(normal);
        if(dot_product(scalar_multiplication(r->ray_dir, -1), normal)<0) normal = scalar_multiplication(normal, -1);

        check_lights(r, intersection, normal, obj_color, this);
        return run_recursion(normal, intersection, obj_color, r, level, t, this);


    }

};

