
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"

const int HEIGHT = 600;
const int WIDTH = 600;
const double PI = 3.14;
const int GSZ = 100;

double ground[GSZ][GSZ] = { 0 };

double offset = 0;
double eyex = 1, eyey = 26 , eyez = 20;
double dy=0.1;
// locomotion
double speed = 0, angular_speed=0;
double dirX, dirZ;
double sight_angle = PI;
// aircraft data
double air_speed = 0, air_angular_speed = 0;
double air_dirX,air_dirY, air_dirZ;
double yaw = PI, pitch=0;
double airX=0, airY=20, airZ=10;

bool slider_is_moving = false;
double oldY;

bool stopBuilding = false;

// materials 
// plastic magenta
float mat1amb[4] = { 0.3,0.3,0.3,0.8 };
float mat1diff[4] = { 0.5,0.5,1,0.8 };
float mat1spec[4] = { 1,1,1,1 };

// silver
float mat2amb[4] = { 0.3,0.3,0.3,0 };
float mat2diff[4] = { 0.6,0.6,0.6,0 };
float mat2spec[4] = { 0.5,0.5,0.5,0 };

// gold
float mat3amb[4] = { 0.3,0.3,0.2,0.5 };
float mat3diff[4] = { 0.75,0.6,0.2,0.5 };
float mat3spec[4] = { 0.8,0.8,0.5,0 };

// lighting
float lt1amb[4] = { 0.3,0.3,0.3,0 };
float lt1diff[4] = { 0.7,0.7,0.7,0 };
float lt1spec[4] = { 0.8,0.8,0.8,0 };
float lt1pos[4] = { 1.5,3,0.8,0 }; // if the last component is 0 the light is DIRECTIONAL
// if the last component is 1 the light is positional

float lt2amb[4] = { 0.1,0.1,0.1,0 };
float lt2diff[4] = { 0.4,0.1,0.0,0 };
float lt2spec[4] = { 0.5,0.5,0.5,0 };
float lt2pos[4] = { -1.5,3,0.8,0 }; // if the last component is 0 the light is DIRECTIONAL
// if the last component is 1 the light is positional

void UpdateGround1();
void UpdateGround2();
void 	Smooth();


void init()
{
	int i, j;

	srand(time(0));

	for(i=1;i<1000;i++)
		UpdateGround2();

	Smooth();
				//  R   G    B
	glClearColor(0.7,0.8,1,0);// color of window background

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

}

// add random heights
void UpdateGround1()
{
	int num_pts = 100;
	int i, j;
	double delta = 0.05;

	if (rand() % 2 == 0)
		delta = -delta;

	for (int count = 1; count <= num_pts; count++)
	{
		i = rand() % GSZ;
		j = rand() % GSZ;
		ground[i][j] += delta;
	}
}

void UpdateGround2()
{
	int x1, y1, x2, y2;
	int i, j;
	double delta = 0.05;
	double a, b;

	if (rand() % 2 == 0)
		delta = -delta;

	x1 = rand() % GSZ;
	y1 = rand() % GSZ;
	x2 = rand() % GSZ;
	y2 = rand() % GSZ;

	if (x1 != x2)
	{
		a = (y2 - y1) / ((double)x2 - x1);
		b = y1 - a * x1;

		for(i=0;i<GSZ;i++)
			for (j = 0; j < GSZ; j++)
			{
				if (i > a * j + b)
					ground[i][j] += delta;
				else
					ground[i][j] -= delta;
			}
	}

}

void UpdateGround3()
{
	int num_pts = 1500;
	int x1, y1, direction;
	double delta = 0.05;

	if (rand() % 2 == 0)
		delta = -delta;

	x1 = rand() % GSZ;
	y1 = rand() % GSZ;


	for (int count = 1; count <= num_pts; count++)
	{
		ground[y1][x1] += delta;
		direction = rand() % 4;
		switch (direction)
		{
		case 0: // up
			y1++;
			break;
		case 1: // right
			x1++;
			break;
		case 2: // down
			y1--;
			break;
		case 3: // left
			x1--;
			break;
		}
		x1 = (x1 + GSZ) % GSZ;
		y1 = (y1 + GSZ) % GSZ;

	}

}

// low-pass filter
void Smooth()
{
	double tmp[GSZ][GSZ];
	int i, j;

	for(i=1;i<GSZ-1;i++)
		for (j = 1; j < GSZ - 1; j++)
		{
			tmp[i][j] = (ground[i-1][j-1]+ 2*ground[i - 1][j]+ ground[i - 1][j + 1]+
								2*ground[i][j - 1] + 4*ground[i ][j] + 2*ground[i][j + 1]+
								ground[i + 1][j - 1] + 2*ground[i + 1][j] + ground[i + 1][j + 1]) / 16.0;
		}

	for (i = 1; i < GSZ - 1; i++)
		for (j = 1; j < GSZ - 1; j++)
			ground[i][j] = tmp[i][j];

}

void SetColor(double h)
{
	// sand
	if (fabs(h) < 0.3)
		glColor3d(0.9, 0.8, 0.7);
	// valley
	else if(fabs(h) <2.5)
		glColor3d(h/5, 0.8-h/4, h/6);
	else if(fabs(h) <4) // stones
		glColor3d(0.3+h / 11,0.3+h /11, 0.3+h / 11);
	else 	// snow
		glColor3d(h / 7, h / 7, h / 5);



}

void SetNormal(int i, int j)
{
	double nx, ny, nz;
	double ax = -1, ay = ground[i][j-1]-ground[i][j], az = 0;
	double bx = 0, by = ground[i-1][j] - ground[i][j], bz = -1;

	nx = ay * bz - az * by;
	ny = ax * bz - az * bx;
	nz = ax * by - ay * bx;

	glNormal3d(nx, ny, nz);
}

void DrawFloor()
{
	int i,j;

	glPushMatrix();
	glTranslated(-GSZ / 2, 0, -GSZ / 2);
	glColor3d(1, 1, 1);
	for(i=2;i<GSZ;i++)
		for (j = 2; j < GSZ; j++)
		{
			glBegin(GL_POLYGON);
//			SetColor(ground[i][j]);
			SetNormal(i, j);
			glVertex3d(j, ground[i][j], i);
//			SetColor(ground[i][j-1]);
			SetNormal(i, j-1);
			glVertex3d(j-1, ground[i][j-1], i);
//			SetColor(ground[i-1][j-1]);
			SetNormal(i-1, j-1);
			glVertex3d(j - 1, ground[i-1][j - 1], i-1);
//			SetColor(ground[i-1][j]);
			SetNormal(i-1, j);
			glVertex3d(j , ground[i - 1][j], i - 1);

			glEnd();
		}
	glPopMatrix();
	glDisable(GL_LIGHTING);
	// add water
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glColor4d(0, 0.3, 0.7,0.5);
	glBegin(GL_POLYGON);
	glVertex3d(-GSZ / 2, 0, -GSZ / 2);
	glVertex3d(-GSZ / 2, 0, GSZ / 2);
	glVertex3d(GSZ / 2, 0, GSZ / 2);
	glVertex3d(GSZ / 2, 0, -GSZ / 2);
	glEnd();
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
}

void DrawAxes()
{
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3d(1, 0, 0);
	glVertex3d(0, 0.1, 0);
	glVertex3d(15, 0.1, 0);

	glColor3d(0, 1, 0);
	glVertex3d(0, 0, 0);
	glVertex3d(0, 8, 0);

	glColor3d(0, 0, 1);
	glVertex3d(0, 0.1, 0);
	glVertex3d(0, 0.1, 15);
	glEnd();
	glLineWidth(1);

}

void DrawPart(double w, double h, int hasWindowOrDoor)
{
	if (hasWindowOrDoor == 0) { // no window or door
		glBegin(GL_POLYGON);
		glVertex3d(-w / 2, 0, 0);
		glVertex3d(-w / 2, h, 0);
		glVertex3d(w / 2, h, 0);
		glVertex3d(w / 2, 0, 0);
		glEnd();
	}
	else if (hasWindowOrDoor == 1) // window
	{
		glBegin(GL_POLYGON);
		glVertex3d(-w / 2, 0, 0);
		glVertex3d(-w / 2, h / 3, 0);
		glVertex3d(w / 2, h / 3, 0);
		glVertex3d(w / 2, 0, 0);
		glEnd();
		glPushMatrix();
		glTranslated(0, 2 * h / 3, 0);
		glBegin(GL_POLYGON);
		glVertex3d(-w / 2, 0, 0);
		glVertex3d(-w / 2, h / 3, 0);
		glVertex3d(w / 2, h / 3, 0);
		glVertex3d(w / 2, 0, 0);
		glEnd();
		glPopMatrix();
	}
	else if (hasWindowOrDoor == 2) // door
	{
		glPushMatrix();
		glTranslated(0, 2 * h / 3, 0);
		glBegin(GL_POLYGON);
		glVertex3d(-w / 2, 0, 0);
		glVertex3d(-w / 2, h / 3, 0);
		glVertex3d(w / 2, h / 3, 0);
		glVertex3d(w / 2, 0, 0);
		glEnd();
		glPopMatrix();

	}



}

void DrawWall(int num_parts)
{
	for (int i = 0; i < num_parts; i++)
	{
		glPushMatrix();
		glTranslated(i+0.5- num_parts/2.0, 0, 0);
		DrawPart(1, 3, i % 2);
		glPopMatrix();
	}
}


void DrawRoof(double height)
{
	glColor3d(0.7, 0.2, 0);
	glBegin(GL_POLYGON);
	glVertex3d(-1, 0, 1);
	glVertex3d(0, height, 1);
	glVertex3d(0, height, -1);
	glVertex3d(-1, 0, -1);
	glEnd();

	glColor3d(0.8, 0.4, 0.2);
	glBegin(GL_POLYGON);
	glVertex3d(1, 0, 1);
	glVertex3d(0, height, 1);
	glVertex3d(0, height, -1);
	glVertex3d(1, 0, -1);
	glEnd();

}

void DrawHouse()
{
	// first wall
	glColor3d(0.8, 0.6, 0.2);
	glPushMatrix();
	glTranslated(0, 0, 2.5);
	DrawWall(5);
	glPopMatrix();
	// second wall
	glColor3d(0.8, 0.6, 0.2);
	glPushMatrix();
	glTranslated(0, 0, -2.5);
	DrawWall(5);
	glPopMatrix();
	// third wall
	glColor3d(0.9, 0.7, 0.4);
	glPushMatrix();
	glRotated(90, 0, 1, 0);
	glTranslated(0, 0, 2.5);
	DrawWall(5);
	glPopMatrix();
	// forth wall
	glColor3d(0.9, 0.7, 0.4);
	glPushMatrix();
	glRotated(-90, 0, 1, 0);
	glTranslated(0, 0, 2.5);
	DrawWall(5);
	glPopMatrix();


	glPushMatrix();
	glTranslated(0, 3, 0);
	glScaled(2.5, 1, 2.5);
	DrawRoof(2);
	glPopMatrix();

}

void DrawColorCube()
{
	// top
	glBegin(GL_POLYGON);
	glColor3d(1, 1, 1); // white
	glVertex3d(1, 1, 1);
	glColor3d(0, 1, 0); // green
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, 1, -1);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, 1, 1);
	glEnd();

	// bottom
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, -1, 1);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, -1, -1);
	glColor3d(0, 0, 0); // black
	glVertex3d(-1, -1, -1);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(-1, -1, 1);
	glEnd();

	// left
	glBegin(GL_LINE_LOOP);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, 1, 1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, 1, -1);
	glColor3d(0, 0, 0); // black
	glVertex3d(-1, -1, -1);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(-1, -1, 1);
	glEnd();

	// right
	glBegin(GL_LINE_LOOP);
	glColor3d(1, 1, 1); // white
	glVertex3d(1, 1, 1);
	glColor3d(0, 1, 0); // green
	glVertex3d(1, 1, -1);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, -1, -1);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, -1, 1);
	glEnd();

	// front
	glBegin(GL_LINE_LOOP);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, 1, 1);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(-1, -1, 1);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, -1, 1);
	glColor3d(1, 1, 1); // white
	glVertex3d(1, 1, 1);
	glEnd();

	// rear
	glBegin(GL_LINE_LOOP);
	glColor3d(0, 1, 0); // green
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, 1, -1);
	glColor3d(0, 0, 0); // black
	glVertex3d(-1, -1, -1);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, -1, -1);
	glEnd();

}

void DrawCylinder(int num_pts)
{
	double alpha, teta = 2 * PI / num_pts;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha)),  1-fabs(sin(2*alpha)), fabs(cos(alpha)));
		glVertex3d(sin(alpha), 1, cos(alpha)); // 1
		glVertex3d(sin(alpha + teta), 1, cos(alpha + teta)); // 2
		glColor3d(0.5*fabs(sin(alpha)), 1-fabs(sin(2 * alpha)), fabs(0.5 * cos(alpha)));
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta  )); // 3
		glVertex3d(sin(alpha ), 0, cos(alpha )); // 4
		glEnd();
	}
}

void DrawCylinder1(int num_pts, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / num_pts;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha)), 1 - fabs(sin(2 * alpha)), fabs(cos(alpha)));
		glVertex3d(topr*sin(alpha), 1, topr * cos(alpha)); // 1
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // 2
		glColor3d(0.5 * fabs(sin(alpha)), 1 - fabs(sin(2 * alpha)), fabs(0.5 * cos(alpha)));
		glVertex3d(bottomr*sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // 3
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // 4
		glEnd();
	}
}

// lighting
void DrawCylinder2(int num_pts, double topr, double bottomr)
{
	double alpha, teta = 2 * PI / num_pts;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glNormal3d(topr * sin(alpha), bottomr * (bottomr - topr), topr * cos(alpha));
		glVertex3d(topr * sin(alpha), 1, topr * cos(alpha)); // 1
		glNormal3d(topr * sin(alpha + teta), bottomr * (bottomr - topr), topr * cos(alpha + teta));
		glVertex3d(topr * sin(alpha + teta), 1, topr * cos(alpha + teta)); // 2
		glNormal3d(bottomr * sin(alpha + teta), bottomr * (bottomr - topr), bottomr * cos(alpha + teta));
		glVertex3d(bottomr * sin(alpha + teta), 0, bottomr * cos(alpha + teta)); // 3
		glNormal3d(bottomr * sin(alpha), bottomr * (bottomr - topr), bottomr * cos(alpha ));
		glVertex3d(bottomr * sin(alpha), 0, bottomr * cos(alpha)); // 4
		glEnd();
	}
}

void DrawSphere(int num_pts,int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = -PI / 2; beta <= PI / 2; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta+delta)-sin(beta), 1);
		DrawCylinder1(num_pts, cos(beta + delta), cos(beta));
		glPopMatrix();
	}
}

void DrawSphere1(int num_pts, int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = -PI / 2; beta <= PI / 2; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder2(num_pts, cos(beta + delta), cos(beta));
		glPopMatrix();
	}
}

void DrawApple(int num_pts, int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = 0; beta <= 2*PI ; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder1(num_pts,1+ cos(beta + delta),1+ cos(beta));
		glPopMatrix();
	}
}

void DrawDonut(int num_pts, int num_slices)
{
	double beta, delta = PI / num_slices;

	for (beta = 0; beta <= 2 * PI; beta += delta)
	{
		glPushMatrix();
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCylinder1(num_pts, 5 + cos(beta + delta), 5 + cos(beta));
		glPopMatrix();
	}
}

void DrawAircraft()
{
	glPushMatrix();
	glRotated(90, 0, 1, 0); // rotate aircraft as unit
	glRotated(90, 0, 0, 1); // rotate aircraft as unit
	// head
	glPushMatrix();
	glTranslated(0, 3, 0);
	glScaled(1, 5, 1);
	DrawCylinder2(30, 0, 1);
	glPopMatrix();
	// body
	glPushMatrix();
	glScaled(1, 3, 1);
	DrawCylinder2(30, 1, 1.2);
	glPopMatrix();

	// back
	glPushMatrix();
	glTranslated(0, -11, 0);
	glScaled(1, 11, 1);
	DrawCylinder2(30, 1.2, 0.5);
	glPopMatrix();

	// cockpit

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat1amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat1diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat1spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

	glEnable(GL_BLEND);

	glPushMatrix();
	glTranslated(0.5, 1, 0);
	glScaled(1, 3, 1);
	DrawSphere1(30, 30);
	glPopMatrix();
	glDisable(GL_BLEND);


	glMaterialfv(GL_FRONT, GL_AMBIENT, mat2amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat2diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat2spec);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	glNormal3d(1, 0, 0);
	// wings
	glBegin(GL_POLYGON);
	glVertex3d(0, 3, 0);
	glVertex3d(0, -4, 5);
	glVertex3d(0, -6, 5);
	glVertex3d(0, -4, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3d(0, 3, 0);
	glVertex3d(0, -4, -5);
	glVertex3d(0, -6, -5);
	glVertex3d(0, -4, 0);
	glEnd();

	// small wings
	glBegin(GL_POLYGON);
	glVertex3d(0, -4, 0);
	glVertex3d(0, -11, 3);
	glVertex3d(0, -12, 3);
	glVertex3d(0, -10, 0);
	glEnd();

	glBegin(GL_POLYGON);
	glVertex3d(0, -4, 0);
	glVertex3d(0, -11, -3);
	glVertex3d(0, -12, -3);
	glVertex3d(0, -10, 0);
	glEnd();
	// tail
	glNormal3d(0, 0, 1);
	glColor3d(0.3, 0.2, 0.6);
	glBegin(GL_POLYGON);
	glVertex3d(0, -3, 0);
	glVertex3d(3.5, -11, 0);
	glVertex3d(3.5, -12, 0);
	glVertex3d(0, -10, 0);
	glEnd();

	glPopMatrix();
}

void DrawSlider()
{
	glColor3d(1, 1, 0);
	// background
	glBegin(GL_POLYGON);
	glVertex2d(0, 0);
	glVertex2d(0, 100);
	glVertex2d(100, 100);
	glVertex2d(100,0 );
	glEnd();

	glLineWidth(3);
	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(50, 0);
	glVertex2d(50, 100);
	glEnd();
	glLineWidth(1);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	glPushMatrix();
	glTranslated(0, pitch*180/PI, 0); // move in dgrees
	// slider button
	glColor3d(0.5, 0.5, 0.5);
	glBegin(GL_POLYGON);
	glVertex2d(40, 40);
	glVertex2d(40, 60);
	glVertex2d(60, 60);
	glVertex2d(60, 40);
	glEnd();

	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(40, 50);
	glVertex2d(60, 50);
	glEnd();

	glPopMatrix();
}

// put all drawings here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z buffer
	glViewport(0, 0, WIDTH, HEIGHT);

	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

	glFrustum(-1, 1, -1, 1, 1, 3000);
	gluLookAt(eyex, eyey, eyez, 
		eyex+dirX, eyey-0.5, eyez+dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// Add lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); // allow light source # 1
	glEnable(GL_LIGHT1); // allow light source # 2

	glLightfv(GL_LIGHT0, GL_AMBIENT, lt1amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lt1diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lt1spec);
	glLightfv(GL_LIGHT0, GL_POSITION, lt1pos);

	glLightfv(GL_LIGHT1, GL_AMBIENT, lt2amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lt2diff);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lt2spec);
	glLightfv(GL_LIGHT1, GL_POSITION, lt2pos);

// add material
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat2amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat2diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat2spec);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);

	/*
	glPushMatrix();
	// cone
	glPushMatrix();
	glColor3d(1, 0, 0);
	glTranslated(0, 15, 0);
	glRotated(3 * offset, 0, 1, 0);


	glRotated(-90, 1, 0, 0);
	glutSolidCone(3, 7, 20, 20);
	glPopMatrix();


	glMaterialfv(GL_FRONT, GL_AMBIENT, mat3amb);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat3diff);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat3spec);
	glMaterialf(GL_FRONT, GL_SHININESS, 120);

	// teapot
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslated(12, 15, 0);

	glRotated(3*offset, 0, 1, 0);

	glutSolidTeapot(5);
	glPopMatrix();
	glDisable(GL_BLEND);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat1amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat1diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat1spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 127);

	// Icosahedron
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTranslated(-12, 15, 0);
	glRotated(3 * offset, 0, 1, 0);
	glScaled(3, 3, 3);
	glutSolidIcosahedron();

	glPopMatrix();
	glDisable(GL_BLEND);
//	glDisable(GL_LIGHTING);
*/

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw*180/PI, 0, 1, 0);

	glRotated(-10000*air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();

	// controls sub-window
	glViewport(0, 0, 100, 100);
	// Now we switch to Orthogonal projection, so we must change the projections matrix
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here
	glOrtho(0, 100, 0, 100, -1, 1);
	glDisable(GL_DEPTH_TEST); // return to principles of graphics 2D

	DrawSlider();

	glEnable(GL_DEPTH_TEST);
	
	glutSwapBuffers(); // show all
}

void displayCombined()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z buffer
	glViewport(0, HEIGHT / 2, WIDTH / 2, HEIGHT / 2);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyex, eyey, eyez,
		eyex + dirX, eyey - 0.5, eyez + dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw * 180 / PI, 0, 1, 0);

	glRotated(-10000 * air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();
	// top
	glViewport(WIDTH/2, HEIGHT/2, WIDTH/2, HEIGHT/2);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyex, 80, eyez,
		eyex + dirX, 70, eyez + dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw * 180 / PI, 0, 1, 0);

	glRotated(-10000 * air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();
// cockpit view
	glViewport(0, 0, WIDTH, HEIGHT/2);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

//	glRotated(-10000 * air_angular_speed, air_dirX, air_dirY, air_dirZ);
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(airX, airY + 2, airZ,
		airX + air_dirX, airY + 1.5 + air_dirY, airZ + air_dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw * 180 / PI, 0, 1, 0);

	glRotated(-10000 * air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();



	glutSwapBuffers(); // show all
}


void displayCockpit()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z buffer
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

//	glRotated(-10000 * air_angular_speed, air_dirX, air_dirY, air_dirZ);
	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(airX, airY+2, airZ,
		airX+air_dirX, airY+1.5 + air_dirY, airZ + air_dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw * 180 / PI, 0, 1, 0);

	glRotated(-10000 * air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();


	glutSwapBuffers(); // show all
}

void displayTop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clean frame buffer and Z buffer
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION); // vision matrix
	glLoadIdentity(); // start transformations here

	glFrustum(-1, 1, -1, 1, 1, 300);
	gluLookAt(eyex, 80, eyez,
		eyex + dirX, 70, eyez + dirZ,
		0, 1, 0);

	glMatrixMode(GL_MODELVIEW); // matrix of world
	glLoadIdentity(); // start transformations here

	DrawFloor();

	// aircraft
	glPushMatrix();

	glTranslated(airX, airY, airZ);
	glRotated(yaw * 180 / PI, 0, 1, 0);

	glRotated(-10000 * air_angular_speed, 0, 0, 1);
	glRotated(-pitch * 180 / PI, 1, 0, 0);
	glScaled(0.5, 0.5, 0.5);
	DrawAircraft();

	glPopMatrix();


	glutSwapBuffers(); // show all
}

void idle() 
{
	int i, j;	
	double dist;
	offset += 0.1;

//	if(!stopBuilding)
//		UpdateGround3();

	// aircraft motion
	yaw += air_angular_speed;
	air_dirX = sin(yaw);
	air_dirZ = cos(yaw);
	air_dirY = sin(pitch);

	airX += air_speed * air_dirX;
	airY += air_speed * air_dirY;
	airZ += air_speed * air_dirZ;
	// player motion
	sight_angle += angular_speed;

	dirX = sin(sight_angle);
	dirZ = cos(sight_angle);

	eyex += speed*dirX;
	eyez += speed*dirZ;
	glutPostRedisplay(); // calls indirectly display
}

void specialFun(int key, int x, int y)
{
	switch (key) 
	{
	case GLUT_KEY_UP:
		speed+=0.01;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.01;
		break;
	case GLUT_KEY_LEFT:
		angular_speed += 0.001;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.001;
		break;
	case GLUT_KEY_PAGE_UP:
		eyey += dy;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eyey -= dy;
		break;

	}
}

void mouse(int button, int state, int x, int y) 
{
	double angle_in_degrees = pitch * 180 / PI+50;
	bool clickInSlider = x > 40 && x<60 && HEIGHT - y >angle_in_degrees-10 && HEIGHT - y < angle_in_degrees + 10;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && clickInSlider) // drag slider
	{
		slider_is_moving = true;
		oldY = HEIGHT-y; 
	}
	else	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		stopBuilding = !stopBuilding;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		slider_is_moving = false;
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'a':
		air_angular_speed += 0.001;
		break;
	case 'w':
		air_speed += 0.01;
		break;
	case 's':
		air_speed -= 0.01;
		break;
	case 'd':
		air_angular_speed -= 0.001;
		break;
	case 'q':
		pitch += 0.1;
		break;
	case 'z':
		pitch -= 0.1;
		break;
	}
}

void menu(int choice)
{
	switch (choice)
	{
	case 1:
		glutDisplayFunc(display);
		break;
	case 2:
		glutDisplayFunc(displayTop);
		break;
	case 3:
		glutDisplayFunc(displayCockpit);
		break;
	case 4:
		glutDisplayFunc(displayCombined);
		break;
	}
}

void mouseDrag(int x, int y)
{
	double dy =  (HEIGHT - y) - oldY;

	if (slider_is_moving && HEIGHT - y<100 && HEIGHT - y>0)
	{
		pitch += dy*PI/180; // transform dy to radian
		oldY = (HEIGHT - y);

	}
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(200, 50);
	glutCreateWindow("Aircraft");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutSpecialFunc(specialFun);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	glutMotionFunc(mouseDrag);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Regular View", 1);
	glutAddMenuEntry("Top View", 2);
	glutAddMenuEntry("Cockpit View", 3);
	glutAddMenuEntry("Combined View", 4);


	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}