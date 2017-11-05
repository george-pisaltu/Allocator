#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char *arena;

void INITIALIZE(unsigned char *arena, int n)
{
	int i;
	*arena=(unsigned char *)malloc(n*sizeof(unsigned char));
	for(i=0;i<n;i++)
	{
		arena[i]=0;
	}
}

void FINALIZE(unsigned char *arena)
{
	free(arena);
}


int ALLOC(unsigned char *arena, int n, unsigned int SIZE, int *b)
{
	int *p,i,*q=&(arena[0]),*r;
	if(*q==0)
	{
		if(SIZE+12>n-4)
		{
			printf("0\n");
			return 1;
		}
		else
		{
			*(q+1)=0;
			*(q+2)=0;
			*(q+3)=SIZE+12;
			*q=4;
			printf("16\n");
			*b=*b+1;
			return 0;
		}
	}
	else
	{
		if(*q-4>=12+SIZE)
		{
			p=&(arena[*q]);
			*(q+1)=*q;
			*(q+2)=0;
			*(q+3)=12+SIZE;
			*(p+1)=4;
			*q=4;
			printf("16\n");
			(*b)++;
			return 0;
		}
		else
		{
			p=&(arena[*q]);
			i=*q;
			q=p;
			while(*p!=0)
			{
				q=&(arena[*p]);
				if(SIZE+12<=*p-*(q+1)-*(p+2))
				{
					r=&(arena[*(p+2)+*(q+1)]);
					*r=*p;
					*(r+1)=*(q+1);
					*(r+2)=SIZE+12;
					*p=*(p+2)+*(q+1);
					*(q+1)=*p;
					printf("%d\n",*p+12);
					(*b)++;
					return 0;
				}
				else
				{
					i=*p;
					p=&arena[*p];
				}
			}
			if(SIZE+12<=n-i-*(p+2))
			{
				r=&arena[i+*(p+2)];
				*r=0;
				*(r+1)=i;
				*(r+2)=12+SIZE;
				*p=i+*(p+2);
				printf("%d\n",*p+12);
				(*b)++;
				return 0;
			}
		}
	}
	printf("0\n");
	return -1;
}

int DUMP(unsigned char *arena, int n)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(i==0)
		{
			printf("%08X\t",i);
		}
		if(i%16==0 && i!=0)
		{
			printf("\n%08X\t",i);
		}
		if((i+8)%16==0)
		{
			printf(" ");
		}
		printf("%02X ",arena[i]);
	}
	printf("\n%08X\n",n);
	return 0;
}


int FILL(unsigned char *arena, int n, int i, int x, int s)
{
	int j;
	for(j=i;j<i+s && j<n;j++)
	{
		arena[j]=x;
	}
	return 0;
}

int FREE(unsigned char *arena, int i, int *c)
{
	int *q,j,*a,*b,size;
	q=&(arena[i-12]);
	size=*(q+2);
	b=&(arena[*q+4]);
	*b=*(q+1);
	a=&(arena[*(q+1)]);
	*a=*q;
	*c=*c-1;
	return 0;
}


int SHOW_FREE(unsigned char *arena, int n, int *b)
{
	int *p,nr=0,nr1=0,i,*q,e,f,ok=0;
	float E,F;
	p=&(arena[0]);
	if(*p==0)
	{
		printf("1 blocks (%d bytes) free\n",n-4);
		return 0;
	}
	if(*p>4)
	{
		nr1++;
	}
	while(*p!=0)
	{
		q=&(arena[*p]);
		nr=nr+*(q+2)-12;
		if(*p-*(q+1)-*(p+2)>0 && ok==1)
		{
			nr1++;
		}
		p=q;
		ok=1;
	}
	q=&(arena[*(p+1)]);
	if(*(q+1)+*(p+2)<n)
	{
		nr1++;
	}
	printf("%d blocks (%d bytes) free\n",nr1,n-nr-4-(*b)*12);
	return 0;
}

int SHOW_USAGE(unsigned char *arena, int n, int *b)
{
	int *p,nr=0,nr1=0,i,*q,e,f,ok=0;
	float E,F;
	p=&(arena[0]);
	if(*p>4)
	{
		nr1++;
	}
	while(*p!=0)
	{
		q=&(arena[*p]);
		nr=nr+*(q+2)-12;
		if(*p-*(q+1)-*(p+2)>0 && ok==1)
		{
			nr1++;
		}
		p=q;
		ok=1;
	}
	q=&(arena[*(p+1)]);
	if(*(q+1)+*(p+2)<n)
	{
		nr1++;
	}
	printf("%d blocks (%d bytes) used\n",*b,nr);
	E=nr;
	E=(E/(E+4.0+(*b)*12))*100;
	e=E;
	printf("%d",e);
	printf("%% efficiency\n");
	F=nr1-1.0;
	F=(F/(*b))*100;
	f=F;
	if(*b==0)
	{
		printf("0");
		printf("%% fragmentation\n");
	}
	else
	{
		printf("%d",f);
		printf("%% fragmentation\n");
	}
	return 0;
}

int SHOW_ALLOCATIONS(unsigned char *arena, int n)
{
	int *p=&(arena[0]),i,*q,d;
	if(*p==0)
	{
		printf("OCCUPIED 4 bytes\nFREE %d bytes\n",n-4);
	}
	else
	{
		i=*p;
		printf("OCCUPIED 4 bytes\n");
		if((*p-4)!=0)
		{
			printf("FREE %d bytes\n",*p-4);
		}
		p=&arena[*p];
		while(*p!=0)
		{
			q=&(arena[*p]);
			d=*p-*(q+1)-*(p+2);
			printf("OCCUPIED %d bytes\n",*(p+2));
			if(d>0)
			{
				printf("FREE %d bytes\n",d);
			}
			i=*p;
			p=q;
		}
		printf("OCCUPIED %d bytes\nFREE %d bytes\n",*(p+2),n-i-*(p+2));
	}
	return 0;
}





int main(){  
	int N, i, exit = 1, SIZE, *b, z=0, s,j, x;
	b=&z;

	char comm[30], command[30];
          
	while(exit!=0){ 
		scanf("%s",command);
		if (strcmp(command,"INITIALIZE")==0){  
			scanf ("%d", &N);  
			INITIALIZE (&arena, N); 
		}
		if (strcmp(command,"ALLOC")==0){ 
			scanf ("%d", &SIZE); 
			ALLOC(&arena, N, SIZE, b);
		} 
		if (strcmp(command,"DUMP")==0){ 
			DUMP(&arena, N);
		} 
		if (strcmp(command,"FREE")==0){ 
			scanf("%d",&i);
			FREE(&arena, i, b);
		}
		if (strcmp(command,"FINALIZE")==0){                   
 			FILL(&arena,N,0,0,N);
 			FINALIZE(arena);
			exit = 0;
		}
		if (strcmp(command,"SHOW")==0)
		{
			scanf("%s",comm);
			if(strcmp(comm,"FREE")==0)
			{
				SHOW_FREE(&arena, N, b);
			}
			if(strcmp(comm,"USAGE")==0)
			{
				SHOW_USAGE(&arena, N, b);
			}
			if(strcmp(comm,"ALLOCATIONS")==0)
			{
				SHOW_ALLOCATIONS(&arena, N);
			}
		}
		if(strcmp(command,"FILL")==0)
		{
			scanf("%d %d %d", &i, &s, &x);
			FILL(&arena, N, i, x, s);
		}
	}
	return 0;
}