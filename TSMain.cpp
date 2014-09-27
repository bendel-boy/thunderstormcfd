#include"TSMain.hpp"


int main(int argc,char *argv[])
{

  Root CgnsRoot;


  FILE *nscon;
  int choice, nVariable, restart, variableTag,nv,counter=1,p;
  double time;
  char strtemporary1[40],strtemporary2[40],strtemporary3[40],strtemporary4[40];
	printf("\n\n..............................Hurricane is Running ............................");
	printf("\nWelcome to Project ThunderStorm: Solver for Hybrid Unstructured Single Zone Incompressible (Laminar and LES) NavierStokes Equations");
	printf("\nPlease enter 0 or 1 when asked by querry so else enter respective values for the querry \n\n");
	
	char *filename1,*filename2,*filename3;
		
	filename1 =(char *)malloc(40*sizeof(char));
	filename2 =(char *)malloc(40*sizeof(char));
	filename3 =(char *)malloc(40*sizeof(char));
	
	
	
	if(argc!=3)
		{
			printf("\nWrong usage...\nfollow the accompanying syntax...\n./run input_file output_file\n");
			exit(0);
		}
	
	strcpy(strtemporary2,argv[1]);//if arguments are proper copy them into input and output filename strings.
	filename1 = strtemporary2;//strcat(strtemporary1,strtemporary2);
	strcpy(strtemporary3,"SOLUTIONS/");
	strcpy(strtemporary4,argv[2]);
	filename2 = strcat(strtemporary3,strtemporary4);
	strcpy(filename3,"SOLUTIONS/restart.cgns");//DO NOT TAMPER WITH THIS NAME
	
						
	CgnsRoot = ReadCgnsFile(filename1); /* Reading cgns grid data file */
        
	filestore(&CgnsRoot,filename1,filename2); //Storing input and output filenames in the Root structure.
	face_node_inf(CgnsRoot.CgnsBase);

  	printf("Do you want to generate Neighbor connectivity? (yes=0, no=1): ");
  	scanf("%d",&choice);
 
  	if(choice==0) 
	{ 
		
		multizoneconnectivity(CgnsRoot.CgnsBase);
		ReadConnectivity(CgnsRoot.CgnsBase);

	} /* Creating neighbor and parent data */
     	else 
	{ 
		
		ReadConnectivity(CgnsRoot.CgnsBase); 
		
	} /* Reading neighbor and parent data */
 
  	restart = InputFunc(CgnsRoot); 

  	GeometricalProperties(CgnsRoot.CgnsBase); /* Calculating geometrical properties for the domain */
	
	CoefficientDiffusion(CgnsRoot.CgnsBase);  /*Calculating the diffusion coefficients */	
 	

 	if(restart==0) /* When the restart is not initialized */
  	{
		nscon=fopen("Settings/converg.dat","w");
		fprintf(nscon,"0");
		fclose(nscon);
		
		nVariable=BoundaryCondInput(CgnsRoot.CgnsBase);	
		
  	}
  	
		
	else if(restart==1) /*When we wish to restart from some previously saved function */
  	{
		nVariable=BoundaryCondInput(CgnsRoot.CgnsBase);
		ReadCgnsFileRestart(filename3, CgnsRoot);
  	}
	

	time=0.0;
	

	       
	for(nv=0;nv<=nVariable;nv++)
  	{
		variableTag = (CgnsRoot.CgnsBase)->VARTAG[nv];
    	printf("VaribleTag=%d\n",variableTag);
		RefreshBoundaryCells(variableTag,CgnsRoot.CgnsBase,time); 

  	}
    
  	if(restart==0)
  	{
    		VolumeFlux(CgnsRoot.CgnsBase);
		
  	}
  	
	
	else if(restart==1)
  	{
    		VolumeFlux(CgnsRoot.CgnsBase);
		
		for(variableTag=1;variableTag<=3;variableTag++)
    		{
      			RefreshMassVelocities(variableTag*100, CgnsRoot.CgnsBase); 
    		}
			
			MassFlux(CgnsRoot.CgnsBase); 
		
  	}

	
	WriteCgnsFile(filename2,CgnsRoot);
	
	WriteCgnsFile(filename3,CgnsRoot);
     	
	SolverSemiImplicitSegregated(CgnsRoot); 

  	WriteCgnsFileModify(filename2,CgnsRoot);
	printf("\nWritten solution in CGNS file...!!!");
	printf("\n.............................. Hurricane Completed ............................");

  return 0;
}
