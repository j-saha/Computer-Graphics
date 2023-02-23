#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include<iostream>
#define pi (2*acos(0.0))
#include <fstream>
#include <string>
#include <stack>
#include <iomanip>
#include "bitmap_image.hpp"
using namespace std;

double radian(double degree)
{
    return degree*pi/180.0;
}

struct point
{
	double x,y,z,w;
};


struct Color{
    double R, G, B;
};
struct transformation
{
    double arr[4][4];
};

struct Triangle{
    point points[3];
    struct Color color;

};

struct Edge
{
    int index_point1;
    int index_point2;
};



point pos;
point eye, look, up;
point u, r, l;
double result_arr[4][1];
double fovY, aspectRatio, near_val, far_val;
stack< transformation > my_stack;
struct transformation initial = { { {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} } } ;
struct transformation identity = initial;
void input_from_file();

string filename_stg1;
string filename_stg2;
string filename_stg3;
string filename_zbuffer;
string filename_scenefile;
string filename_configfile;
string filename_imagefile;
struct transformation V;
int no_triangle=0;
double l_lim_x, r_lim_x, b_lim_y, t_lim_y, f_lim_z, r_lim_z, dx, dy, Top_Y, Left_X;
int Screen_Width, Screen_Height;
double** z_buffer;
struct Color** frame_buffer;
void initialize_stack()
{

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if(i==j) identity.arr[i][j] = 1;
        }
    }
    my_stack.push(identity);
}

void print_transformation(struct transformation trans)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << std::fixed<< std::setprecision(7);
            cout << trans.arr[i][j] << ' ';
        }
        cout << endl;
    }
}


void print_result(double arr[4][1], string filename)
{
    ofstream file;
    file.open (filename, ios::app);
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 1; j++)
        {
            std::cout << std::fixed<< std::setprecision(7);
            cout << arr[j][i]/arr[3][0] << " ";

            file << std::fixed<< std::setprecision(7);
            file << arr[j][i]/arr[3][0] << " ";
        }
    }
    cout<<endl;
    file<<endl;
    file.close();

}

void set_view_matrix(double arr1[4][4], double arr2[4][4])
{
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            V.arr[i][j] = 0;

            for (int k = 0; k < 4; k++) {
                V.arr[i][j] += arr1[i][k] * arr2[k][j];
            }
        }
    }

}

void make_transformation(double x, double y, double z, double arr[4][4])
{

    double tmp_triangle[4][1] = {{x},{y},{z},{1}};
    //print_result(tmp_triangle);
    //print_transformation(my_stack.top());
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 1; j++) {
            result_arr[i][j] = 0.0;

            for (int k = 0; k < 4; k++) {
                result_arr[i][j] += arr[i][k] * tmp_triangle[k][j];
            }
        }
    }


}

void add_operation(struct transformation temp)
{
    struct transformation new_top;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            new_top.arr[i][j] = 0;


            for (int k = 0; k < 4; k++) {
                new_top.arr[i][j] += my_stack.top().arr[i][k] * temp.arr[k][j];
            }

        }
    }


    my_stack.push(new_top);
}

void free_memory()
{
    for(int i=0; i<Screen_Height; i++)
    {
        delete[] z_buffer[i];
    }
    delete[] z_buffer;

    for(int j=0; j<Screen_Height; j++)
    {
        delete[] frame_buffer[j];
    }
    delete[] frame_buffer;

}



void print_point(point p)
{
    std::cout << std::fixed<< std::setprecision(7);
    cout<<p.x<<"  "<<p.y<<"  "<<p.z<<endl;
}

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

double dot_product(point a, point b)
{
    //print_point(a);
    //print_point(b);
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

point scalar_multiplication(point p, double n)
{
    point c;
    c.x = n*p.x;
    c.y = n*p.y;
    c.z = n*p.z;
    return c;
}

point Rodrigues(double ux, double uy, double uz, double theta, double ax, double ay, double az)
{
    point a, x;
    x.x=ux; x.y=uy; x.z=uz;
    a.x=ax; a.y=ay; a.z=az;
    //print_point(a);
    //print_point(b);
    //cout<<(dot_product(a, x))<<"her"<<endl;

    //print_point(scalar_multiplication(a, ((1-cos(radian(theta)))*(dot_product(a, x)))));
    return add_point(add_point(scalar_multiplication(x, cos(radian(theta))), scalar_multiplication(a, ((1-cos(radian(theta)))*(dot_product(a, x))))), scalar_multiplication(cross_product(a, x), sin(radian(theta))));

}


point normalize(point p)
{

    double d = sqrt(p.x*p.x + p.y*p.y + p.z*p.z);
    if(d==0) return p;

    point c;
    c.x = p.x/d;
    c.y = p.y/d;
    c.z = p.z/d;
    return c;


}




void input_from_file()
{
    ifstream scene_file;

    scene_file.open(filename_scenefile);



    scene_file>>eye.x>>eye.y>>eye.z;
    scene_file>>look.x>>look.y>>look.z;
    scene_file>>up.x>>up.y>>up.z;
    scene_file>>fovY>>aspectRatio>>near_val>>far_val;


    initialize_stack();

    int push_arr[10000];
    int push_count = 0;
    int op_count = 0;

    while(true)
    {
        string input_msg;
        scene_file>>input_msg;
        //cout<<input_msg<<endl;


        if(input_msg=="triangle")
        {
            double x, y, z;
            for(int i=0; i<3; i++)
            {
                scene_file>>x>>y>>z;
                make_transformation(x, y, z, my_stack.top().arr);
                print_result(result_arr, filename_stg1);

            }
            cout<<endl;
            ofstream file;
            file.open (filename_stg1, ios::app);
            file<<endl;
            file.close();
            no_triangle++;

        }
        else if(input_msg=="translate")
        {
            struct transformation temp = identity;
            scene_file>>temp.arr[0][3]>>temp.arr[1][3]>>temp.arr[2][3];
            add_operation(temp);
            op_count++;


        }
        else if(input_msg=="scale")
        {
            struct transformation temp = identity;

            scene_file>>temp.arr[0][0]>>temp.arr[1][1]>>temp.arr[2][2];
            //print_transformation(temp);
            add_operation(temp);
            op_count++;

        }
        else if(input_msg=="rotate")
        {
            double degree;
            point a;
            scene_file>>degree>>a.x>>a.y>>a.z;
            a=normalize(a);
            point c1 = Rodrigues(1,0,0,degree,a.x,a.y,a.z);
            point c2 = Rodrigues(0,1,0,degree,a.x,a.y,a.z);
            point c3 = Rodrigues(0,0,1,degree,a.x,a.y,a.z);


            struct transformation temp = identity;
            temp.arr[0][0] = c1.x;
            temp.arr[0][1] = c2.x;
            temp.arr[0][2] = c3.x;
            temp.arr[1][0] = c1.y;
            temp.arr[1][1] = c2.y;
            temp.arr[1][2] = c3.y;
            temp.arr[2][0] = c1.z;
            temp.arr[2][1] = c2.z;
            temp.arr[2][2] = c3.z;

            add_operation(temp);

            op_count++;
            //print_transformation(temp);



        }
        else if(input_msg=="push")
        {
            push_arr[push_count++] = op_count;
            //cout<<"stack size"<<"             "<<my_stack.size()<<endl;

        }
        else if(input_msg=="pop")
        {
            if(push_count<=0)
            {
                cout<<"Stack empty, can't Pop!"<<endl;
            }
            else
            {
                int pop_count = op_count - push_arr[push_count-1];
                //cout<<pop_count<<endl;
                for(int i=0; i<pop_count; i++)
                {
                    my_stack.pop();
                    op_count--;
                }
                push_count--;
                //cout<<"stack size"<<"             "<<my_stack.size()<<endl;

            }


        }
        else if(input_msg=="end")
        {
            break;
        }
        else
        {
            break;
        }


    }



}
void view_transformation()
{
    l = sub_point(look, eye);
    l = normalize(l);
    r = cross_product(l, up);
    r = normalize(r);
    u = cross_product(r, l);

    struct transformation T = identity;
    T.arr[0][3] = -eye.x;
    T.arr[1][3] = -eye.y;
    T.arr[2][3] = -eye.z;

    struct transformation R = identity;
    R.arr[0][0] = r.x;
    R.arr[0][1] = r.y;
    R.arr[0][2] = r.z;
    R.arr[1][0] = u.x;
    R.arr[1][1] = u.y;
    R.arr[1][2] = u.z;
    R.arr[2][0] = -l.x;
    R.arr[2][1] = -l.y;
    R.arr[2][2] = -l.z;


    set_view_matrix(R.arr, T.arr);
    ifstream tmp_file;
    tmp_file.open(filename_stg1);

    for(int i=0; i<no_triangle; i++)
    {
        for(int j=0; j<3; j++)
        {
            double x, y, z;
            tmp_file>>x>>y>>z;
            make_transformation(x, y, z, V.arr);
            print_result(result_arr, filename_stg2);
        }
        cout<<endl;
        ofstream file;
        file.open (filename_stg2, ios::app);
        file<<endl;
        file.close();
    }
}

void projection_transformation()
{
    double fovX = fovY * aspectRatio;
    double t = near_val * tan(radian(fovY/2));
    double r = near_val * tan(radian(fovX/2));

    struct transformation P = initial;
    P.arr[0][0] = near_val/r;
    P.arr[1][1] = near_val/t;
    P.arr[2][2] = -(far_val+near_val)/(far_val-near_val);
    P.arr[2][3] = -(2*far_val*near_val)/(far_val-near_val);
    P.arr[3][2] = -1;


    ifstream tmp_file;
    tmp_file.open(filename_stg2);

    for(int i=0; i<no_triangle; i++)
    {
        for(int j=0; j<3; j++)
        {
            double x, y, z;
            tmp_file>>x>>y>>z;
            make_transformation(x, y, z, P.arr);
            print_result(result_arr, filename_stg3);
        }
        cout<<endl;
        ofstream file;
        file.open (filename_stg3, ios::app);
        file<<endl;
        file.close();
    }



}

void initialize_buffers()
{
    struct Color black;
    black.R=0; black.G=0; black.B=0;

    z_buffer = new double*[Screen_Height];
    frame_buffer = new Color*[Screen_Height];

    for (int i = 0; i < Screen_Height; i++) {

        z_buffer[i] = new double[Screen_Width];
        frame_buffer[i] = new Color[Screen_Width];
    }


    for (int i = 0; i < Screen_Height; i++) {
        for (int j = 0; j < Screen_Width; j++) {
            z_buffer[i][j] = r_lim_z;
            frame_buffer[i][j] = black;
        }
    }
}

int get_top_scanline(struct Triangle triangle)
{
    double t_y;
    if(triangle.points[0].y >= triangle.points[1].y && triangle.points[0].y >= triangle.points[2].y)
        t_y = triangle.points[0].y;

    if(triangle.points[1].y >= triangle.points[0].y && triangle.points[1].y >= triangle.points[2].y)
        t_y = triangle.points[1].y;

    if(triangle.points[2].y >= triangle.points[0].y && triangle.points[2].y >= triangle.points[1].y)
        t_y = triangle.points[2].y;


    double diff = Top_Y - t_y;
    if(diff>0)
    {
        return round(diff/dy);
    }
    return 0;
}
int get_bottom_scanline(struct Triangle triangle)
{
    double b_y;
    if(triangle.points[0].y <= triangle.points[1].y && triangle.points[0].y <= triangle.points[2].y)
        b_y = triangle.points[0].y;

    if(triangle.points[1].y <= triangle.points[0].y && triangle.points[1].y <= triangle.points[2].y)
        b_y = triangle.points[1].y;

    if(triangle.points[2].y <= triangle.points[0].y && triangle.points[2].y <= triangle.points[1].y)
        b_y = triangle.points[2].y;


    double diff = b_y - (b_lim_y+dy/2.0);
    if(diff>0)
    {
        return Screen_Height - round(diff/dy) - 1;
    }
    return Screen_Height - 1;
}

int get_left_intersecting_colm(double min_x)
{

    double diff = min_x - Left_X;

    if(diff>0)
    {

        return round(diff/dx);

    }
    return 0;
}
int get_right_intersecting_colm(double max_x)
{

    double diff = (r_lim_x-dx/2.0) - max_x;
    if(diff>0)
    {
        return Screen_Width - round(diff/dx) - 1;
    }
    return Screen_Width - 1;
}
struct point point_of_intersection[2];
struct Edge minmax_edge[2];
struct Edge min_edge;
struct Edge max_edge;
int valid_intersection;

bool in_between(double num1, double num2, double num)
{
    double max_num = max(num1, num2);
    double min_num = min(num1, num2);
    if(num>=min_num && num<=max_num) return true;
    return false;
}

void get_intersecting_x(struct point point_1, struct point point_2, double ys, int idx1, int idx2)
{
    if(point_2.y - point_1.y == 0) return;
    if(!in_between(point_1.y, point_2.y, ys)) return;

    point_of_intersection[valid_intersection].x = (((point_1.x)*(point_2.y - ys)) + ((point_2.x)*(ys - point_1.y)))/(point_2.y - point_1.y);
    point_of_intersection[valid_intersection].y = ys;

    minmax_edge[valid_intersection].index_point1 = idx1;
    minmax_edge[valid_intersection].index_point2 = idx2;
    valid_intersection++;


}
void generate_image()
{
    bitmap_image image(Screen_Width, Screen_Height);

    for(int c=0;c<Screen_Width;c++){
        for(int r=0;r<Screen_Height;r++){
            image.set_pixel(c,r,frame_buffer[r][c].R,frame_buffer[r][c].G,frame_buffer[r][c].B);
        }
    }

    image.save_image(filename_imagefile);
}

double get_minimum(){

   double temp = point_of_intersection[0].x;
   min_edge = minmax_edge[0];
   //cout<<"no valid "<<valid_intersection<<endl;
   //struct Edge temp_max = minmax_edge[1];
   for(int i=1; i<valid_intersection; i++) {
      if(temp>point_of_intersection[i].x) {
         temp=point_of_intersection[i].x;
         min_edge = minmax_edge[i];
      }
   }

   // cout<<"min  "<<temp<<endl;
   return temp;
}
double get_maximum(){
   double temp = point_of_intersection[0].x;
   max_edge = minmax_edge[0];
   for(int i=1; i<valid_intersection; i++) {
      if(temp<point_of_intersection[i].x) {
         temp=point_of_intersection[i].x;
         max_edge = minmax_edge[i];
      }
   }
   return temp;
}

void clipping_and_scan_conversion()
{
    struct Triangle triangles[no_triangle];
    ifstream config_file;

    config_file.open(filename_configfile);
    config_file>>Screen_Width>>Screen_Height;
    config_file>>l_lim_x; r_lim_x = -l_lim_x;
    config_file>>b_lim_y; t_lim_y = -b_lim_y;
    config_file>>f_lim_z>>r_lim_z;

    ifstream tmp_file;
    tmp_file.open(filename_stg3);

    for(int i=0; i<no_triangle; i++)
    {
        for(int j=0; j<3; j++)
        {
            tmp_file>>triangles[i].points[j].x>>triangles[i].points[j].y>>triangles[i].points[j].z;
        }
        triangles[i].color.R = rand() % 256;
        triangles[i].color.G = rand() % 256;
        triangles[i].color.B = rand() % 256;
    }

    initialize_buffers();
    dx = (r_lim_x-l_lim_x)/Screen_Width;
    dy = (t_lim_y-b_lim_y)/Screen_Height;
    Top_Y = t_lim_y-dy/2.0;
    Left_X = l_lim_x+dx/2.0;

    for(int i=0; i<no_triangle; i++)
    {
        int top_scanline = get_top_scanline(triangles[i]);
        int bottom_scanline = get_bottom_scanline(triangles[i]);

        for(int row = top_scanline; row<bottom_scanline+1; row++)
        {
            valid_intersection = 0;


            double ys = Top_Y - row*dy;
            //for each edge
            //edge 1
            //intersection of edge0, edge1 and y = ys


            get_intersecting_x(triangles[i].points[0], triangles[i].points[1], ys, 0, 1);
            get_intersecting_x(triangles[i].points[0], triangles[i].points[2], ys, 0, 2);
            get_intersecting_x(triangles[i].points[1], triangles[i].points[2], ys, 1, 2);
            if(valid_intersection==0)
            {
                continue;
            }


            double x_min = get_minimum();
            double x_max = get_maximum();
            //printf("%.6g\n", x_max);


            int left_intersecting_colm = get_left_intersecting_colm(x_min);
            int right_intersecting_colm = get_right_intersecting_colm(x_max);
             //cout<<"leftcol in my "<<left_intersecting_colm<<endl;
            //cout<<"leftcol in my"<<right_intersecting_colm<<endl;

            //min_edge = Z1, Z2

            double Za = triangles[i].points[min_edge.index_point1].z + (((ys - triangles[i].points[min_edge.index_point1].y)*(triangles[i].points[min_edge.index_point2].z - triangles[i].points[min_edge.index_point1].z))/(triangles[i].points[min_edge.index_point2].y-triangles[i].points[min_edge.index_point1].y));
            double Zb = triangles[i].points[max_edge.index_point1].z + (((ys - triangles[i].points[max_edge.index_point1].y)*(triangles[i].points[max_edge.index_point2].z - triangles[i].points[max_edge.index_point1].z))/(triangles[i].points[max_edge.index_point2].y-triangles[i].points[max_edge.index_point1].y));
            //cout<<"za "<<Zb<<endl;


            double Zp = Za + (((Left_X + left_intersecting_colm*dx)-x_min)*(Zb-Za))/(x_max-x_min);
//cout<<"herasasee  "<<z_buffer[0][0]<<endl;
            if(z_buffer[row][left_intersecting_colm]>Zp && Zp>=f_lim_z && left_intersecting_colm<=right_intersecting_colm)
            {
                //cout<<"heree"<<endl;
                z_buffer[row][left_intersecting_colm]=Zp;
                frame_buffer[row][left_intersecting_colm].R = triangles[i].color.R;
                frame_buffer[row][left_intersecting_colm].G = triangles[i].color.G;
                frame_buffer[row][left_intersecting_colm].B = triangles[i].color.B;
            }
            double Zprevious = Zp;
            //printf("%.6g\n", Zp);
            //cout<<right_intersecting_colm<<endl;


            for(int colm = left_intersecting_colm+1; colm<right_intersecting_colm+1; colm++)
            {
                //cout<<"inn"<<endl;
                Zp = Zprevious + ((dx)*(Zb-Za))/(x_max-x_min);
                //printf("%.6g\n", Zp);
                Zprevious = Zp;
                if(Zp<f_lim_z) continue;

                if(z_buffer[row][colm]>Zp)
                {
                    z_buffer[row][colm]=Zp;
                    frame_buffer[row][colm].R = triangles[i].color.R;
                    frame_buffer[row][colm].G = triangles[i].color.G;
                    frame_buffer[row][colm].B = triangles[i].color.B;
                }
            }





        }

    }

    ofstream buffer_file;
    buffer_file.open (filename_zbuffer, ios::app);

    for(int m = 0; m<Screen_Height; m++)
    {
        for(int n = 0; n<Screen_Width; n++)
        {
            if(r_lim_z > z_buffer[m][n])
            {
                buffer_file << std::fixed<< std::setprecision(6);
                buffer_file << z_buffer[m][n]<< "	";
            }

        }
        buffer_file<<endl;
    }
    buffer_file.close();







}

void initialize_file()
{
    filename_stg1 = "stage1.txt";
    filename_stg2 = "stage2.txt";
    filename_stg3 = "stage3.txt";
    filename_zbuffer = "z_buffer.txt";
    filename_imagefile = "out.bmp";
    filename_scenefile = "scene.txt";
    filename_configfile = "config.txt";
    remove(filename_stg1.c_str());
    remove(filename_stg2.c_str());
    remove(filename_stg3.c_str());
    remove(filename_zbuffer.c_str());
    remove(filename_imagefile.c_str());
}

int main(){
    srand(time(NULL));
    initialize_file();
    input_from_file();
    view_transformation();
    projection_transformation();
    clipping_and_scan_conversion();
    generate_image();
    free_memory();


    return 0;
}
