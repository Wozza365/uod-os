// not working
// an attempt to draw a 3D cube
// abandoned due to a number of float/int conversion errors
// -m-nosse did not solve these issues.
// not included in build

#include <draw3d.h>
#include <math.h>

void DrawTeapot()
{
    float rotationCos = cos(cube.rotate);
    float rotationSin = sin(cube.rotate);

    uint16_t    projX,
                projY,
                i;

    Vector3 newPos = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

    for (i = 0; i < TOTAL_POINTS; i++)
    {
        Vector3 p = cube.points[i];
        newPos.x = p.x;
        newPos.y = (p.y * rotationCos - p.z * rotationSin);
        newPos.z = (p.y * rotationSin + p.z * rotationCos);
        // uint16_t a = newPos.y;
        // char* c = ToStringUint16(a, 10);
        // WriteText(c, 0,0,1,5);

        newPos.z += cube.z;

        float pX = newPos.x / newPos.z;
        projX = (uint16_t)pX;
        float pY = newPos.y / newPos.z;
        projY = (uint16_t)pY;

        // char *j = ToStringUint16(projX, 10);
        // WriteText(j, 50, i * 10, 1, 5);

        // j = ToStringUint16(projY, 10);
        // WriteText(j, 100, i * 10, 1, 5);

        cube.screenPoints[i].x = projX + (SCREEN_WIDTH / 2);
        cube.screenPoints[i].y = SCREEN_HEIGHT - (projY + (SCREEN_HEIGHT / 2));
    }

    return;

    Vector2 start = {.x = 0, .y = 0};
    Vector2 end = {.x = 0, .y = 0};
    for (i = 0; i < TOTAL_FACES; i++)
    {
        //asm volatile("xchgw %bx, %bx");
        uint16_t face1 = cube.face1[i] - 1,
                 face2 = cube.face2[i] - 1,
                 face3 = cube.face3[i] - 1;

        uint16_t x = cube.screenPoints[face1].x + 20;
        uint16_t y = cube.screenPoints[face1].y + 20;

        // s.y = e.y = cube.points[face1].x;
        // //DrawLine(s, e, 6);
        // s.y = e.y = cube.points[face1].y;
        //DrawLine(s, e, 7);

        start.x = x;
        start.y = y;

        x = cube.screenPoints[face2].x;
        y = cube.screenPoints[face2].y;

        end.x = x;
        end.y = y;
        //DrawLine(s, e, 8);

        //DrawLine(start, end, 5);

        // s.y = e.y = 50;
        // DrawLine(s, e, 7);

        // char *a = ToStringUint16(start.x, 10);
        // WriteText(a, 50, 0, 1, 5);
        // a = ToStringUint16(start.y, 10);
        // WriteText(a, 80, 0, 1, 5);
        // a = ToStringUint16(end.x, 10);
        // WriteText(a, 110, 0, 1, 5);
        // a = ToStringUint16(end.y, 10);
        // WriteText(a, 140, 0, 1, 5);

        x = cube.screenPoints[face2].x;
        y = cube.screenPoints[face2].y;

        start.x = x + 10;
        start.y = y + 10;

        x = cube.screenPoints[face3].x + 20;
        y = cube.screenPoints[face3].y + 20;

        end.x = x + 10;
        end.y = y + 10;

        // start.x = cube.points[face2].x;
        // start.y = cube.points[face2].y;
        // end.x = cube.points[face3].x;
        // end.y = cube.points[face3].y;
        DrawLine(start, end, 5);
        start.x = cube.points[face3].x;
        start.y = cube.points[face3].y;
        end.x = cube.points[face1].x;
        end.y = cube.points[face1].y;
        DrawLine(start, end, 5);
    }

    cube.rotate++;
}