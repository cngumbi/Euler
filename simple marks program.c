#include <stdio.h>
#include <stdlib.h>
int  marks[5];
double meanGrade(float sum);
void displayDistributionOfGrade(void);
int checkMarks(float marks);
int checkHighestMarks();
int checkLowestMarks();
int main(){
        int i,j,highest;
        float sum = 0;
        printf("Enter marks obtained in 5 subjects\n");
        for(i=0;i<5;i++){
            scanf("%d",&marks[i]);
            
            sum=sum + marks[i];
            
        }
        system("cls");
        checkHighestGradAndMarks();
        checkLowestMarks();        
        meanGrade(sum);
        
        return 0;
        
}
/**************************************************
function to display distribution of grade
**************************************************/
void displayDistributionOfGrade(void){
	printf("----------------------------\n");
	printf("Grade Distribution:\n");
	printf("A 80-100\n");
	printf("B 60-80\n");
	printf("C 40-60\n");
	printf("D Below 40");
	
	
}
/***********************************************
function to calculate the mean
***********************************************/
double meanGrade(float sum){
	float avrg;
	avrg=sum/5;
	checkMarks(avrg);
	
	return avrg;
}
/***********************************************
function to check higher grade and marks
***********************************************/
int checkMarks(float marks){
	printf("your Mean grade is \n");
    if(marks>80 && marks<=100){
		printf("A marks: %.2f\n",marks);
	}            
    else if(marks>60&&marks<=80){
		printf("B marks: %.2f\n",marks);
	}
    else if (marks>40&&marks<=60){
		printf("C marks: %.2f\n",marks);
	}            
    else{
		printf("D marks: %.2f\n",marks);	
	} 
   displayDistributionOfGrade();
   getch();
   
   return 0;
}
/**********************************************
function to check the highest grade and marks
**********************************************/
int checkHighestGradAndMarks(){
	printf("------------------------------\n");
	if(marks[0]>marks[1]&&marks[0]>marks[2]&&marks[0]>marks[3]&&marks[0]>marks[4]){
		
		printf("Highest marks is %d\n",marks[0]);
				
	}        
    else if(marks[1]>marks[0]&&marks[1]>marks[2]&&marks[1]>marks[3]&&marks[1]>marks[4]){
    	printf("Highest marks is %d\n",marks[1]);
	}        
    else if(marks[2]>marks[0]&&marks[2]>marks[1]&&marks[2]>marks[3]&&marks[2]>marks[4]){
    	printf("Highest marks is %d\n",marks[2]);
	}   
    else if(marks[3]>marks[0]&&marks[3]>marks[1]&&marks[3]>marks[2]&&marks[3]>marks[4]){
    	printf("Highest marks is %d\n",marks[3]);
	}        
    else{
    	printf("Highest marks is %d\n",marks[4]);
	}     
    printf("------------------------------\n");
    
    return 0;
}
/***************************************************
function to check the lowest marks
****************************************************/
int checkLowestMarks(){
	if(marks[0]<marks[1]&&marks[0]<marks[2]&&marks[0]<marks[3]&&marks[0]<marks[4]){
		printf("Lowest marks is %d\n",marks[0]);		
	}        
    else if(marks[1]<marks[0]&&marks[1]<marks[2]&&marks[1]<marks[3]&&marks[1]<marks[4]){
    	printf("Lowest marks is %d\n",marks[1]);
	}        
    else if(marks[2]<marks[0]&&marks[2]<marks[1]&&marks[2]<marks[3]&&marks[2]<marks[4]){
    	printf("Lowest marks is %d\n",marks[2]);
	}   
    else if(marks[3]<marks[0]&&marks[3]<marks[1]&&marks[3]<marks[2]&&marks[3]<marks[4]){
    	printf("Lowest marks is %d\n",marks[3]);
	}        
    else{
    	printf("Lowest marks is %d\n",marks[4]);
	}     
    printf("------------------------------\n");
    
    return 0;
}
