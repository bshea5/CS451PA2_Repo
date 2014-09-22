//------------------------------------------------------------------------------
//  Copyright 2007-2014 by Jyh-Ming Lien and George Mason University
//  See the file "LICENSE" for more information
//------------------------------------------------------------------------------


#include "main.h"

//
//
//
//
//   The MAIN function
//
//
//
//

int main(int argc, char ** argv)
{
    //
    if(!parseArg(argc,argv)){
        printUsage(argv[0]);
        return 1;
    }

	//read obj vile
	readfromfiles();
	//

    /////////////////////////////////////////////////////////////////
    //setup glut/gli
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH );
    glutInitWindowSize( 680, 680);
    glutInitWindowPosition( 50, 50 );
    glutCreateWindow( "ffd - free-form deformation" );

    InitGL();
    gli::gliInit();
    gli::gliDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
	gli::gliMotionFunc(Motion);
	gli::gliMouseFunc(Mouse);
	glutPassiveMotionFunc(PassiveMotion);

    //set camera position
    gli::setCameraPosZ(R*2.1f);


	user_spring.doInit(FFD_lattice);

    /////////////////////////////////////////////////////////////
    gli::gliMainLoop();

    return 0;
}

