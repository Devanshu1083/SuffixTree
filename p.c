#include<stdio.h>
#include<stdlib.h>
#include<string.h>
typedef struct node{
  struct node *child[256];
  struct node *suffixlink;
  int start;
  int *end;
  int suffixindex;} node;

int leafend = -1,activelength = 0,activeEdge = -1,remainingsuffix=0,size=-1;
int *rootend=NULL;
int *internalend=NULL;
node *activenode=NULL;
node *lastnew=NULL;
node*root=NULL;
char text[200];
node *newnode(int start, int *end)
{
    node *temp=(node*)malloc(sizeof(node));
    temp->start=start;
    temp->end =end;
    temp->suffixindex=-1;
    temp->suffixlink=root;
    for(int i=0;i<256;i++)
    {
        temp->child[i]=NULL;
    }
    return temp;
}
int edgelength(node *n)
{
   if(n==root)
   return 0;

   else
   return *(n->end)-(n->start)+1;
 }
 void setsuffixindexofleaves(node *n,int length)
 {
    if(n==NULL)
    return;

    int leaf=1;
    for(int i=0;i<256;i++)
    {
      if(n->child[i]!=NULL)
       { leaf=0;
        setsuffixindexofleaves(n->child[i], length + edgelength(n->child[i]));
       }
    }

   if(leaf==1)
   {
    n->suffixindex=size-length;
   }
 }
 int walkdown(node *n)
 {
  if(activelength>=edgelength(n))
  {
    activelength=activelength-edgelength(n);
    activenode=n;
    activeEdge=(int)text[activeEdge + edgelength(n)]-(int)' ';
    return 1;
  }
  return 0;
 }
 void extendtree(int phase)
 {
    remainingsuffix++;
    leafend=phase;
    ///first rule extension done by changind end of leaf nodes/////
    lastnew=NULL;
    ////in each new phase initially no node is waiting for a suffix link

    ///now addding suffix remaining using rule 2 or rule 3//////////
    while(remainingsuffix>0)
    {
       if(activelength==0)
       {
        ///rules of activepoint changes
        activeEdge=(int)text[phase]-(int)' ';
        }

        if(activenode->child[activeEdge]==NULL)
        {
            //Extension rule  2 will be applicable here as there is no outgoing edge
            activenode->child[activeEdge]=newnode(phase,&leafend);

          if(lastnew!=NULL)
          {
            lastnew->suffixlink=activenode;
            lastnew=NULL;
          }
        }

        else
        {
           node *next = activenode->child[activeEdge];
           if(walkdown(next))
           {
            continue;
           }
        ///rule 3 applicable if current phase character already  on the edge
          if(text[next->start + activelength]==text[phase])
          {
            if(lastnew!=NULL && activenode!=root)
            {
                lastnew->suffixlink=activenode;
                lastnew=NULL;
            }
            activelength++;
            break;
          }
          /////we reach here if the next character did not match the character we wanted so now we will create an internal node and a leaf
          //node 
          internalend=(int *)malloc(sizeof(int));
          *internalend = next->start + activelength - 1;

          node *internal = newnode(next->start,internalend);
          activenode->child[activeEdge]= internal;
          next->start += activelength;
          internal->child[(int)text[phase]-(int)' ']=newnode(phase,&leafend);
          internal->child[activeEdge] = next;
          ///internal node created so now we chwck for suffix link 
          if(lastnew!=NULL)
          {
            lastnew->suffixlink=internal;
          }
          lastnew=internal;
        

        }
        remainingsuffix--;
        if(activenode==root && activelength>0)
        {
          activeEdge=(int)text[phase -remainingsuffix + 1]-(int)' ';
          activelength--; 
        }
        else if(activenode!=root)
        {
          activenode=activenode->suffixlink;
        }
        
    }

 }
 int edgetraverse(char *s,int k,int start,int end)
 {
    for(int i=start;i<=end && s[k]!='\0';i++,k++)
    {
      if(text[i]!=s[k])
      return -1;
    }
    if(s[k] == '\0')
    return 1;
    else
    return 0;
 }
 void buildsuffixtree()
 {
  size=strlen(text);
  rootend=(int*)malloc(sizeof(int));
  *rootend=-1;
  root=newnode(-1,rootend);
  activenode=root;

  for(int i=0;i<size;i++)
  {
    extendtree(i);
  }
  int length=0;
  setsuffixindexofleaves(root,length);
 }
 /*This function frees the suffix tree allocated memory
 at each recursion it  checks if that node has child or not and if node does 
 then it recurses on that child and frees the nodes also for internal nodes it also 
 frees the end variable.*/
void freetree(node *n)
{
	if(n==NULL)
		{return;}

	for (int i=0;i<256; i++)
	{
		if (n->child[i] != NULL)
		{
			freetree(n->child[i]);
		}
	}
	if (n->suffixindex==-1)
	free(n->end);

	free(n);
}
void traverse1(node *n,int height,int *maxheight,int *index)
{
  if(n == NULL)
  return;
   
   if(n->suffixindex==-1)
   {
    for(int i=0;i<256;i++)
    {
      if(n->child[i] != NULL)
            {
                traverse1(n->child[i], height +  edgelength(n->child[i]), maxheight,index);
            }
    }
   }
   else if(n->suffixindex > -1 &&
                (*maxheight < height - edgelength(n)))
    {
        *maxheight = height - edgelength(n);
        *index = n->suffixindex;
    }
}
void repeat()
{
    int maxheight = 0;
    //index stores the position from where the longest repeated substring starts.
    int index = 0;
    traverse1(root, 0, &maxheight, &index);
    if(maxheight!=0)
    {
      printf("Longest repeated substring in <%s> is: ", text);
    for (int k=0; k<maxheight; k++)
        {printf("%c", text[k + index]);}
    }
    else
    printf("No repeated substrings found in text <%s>.",text);
    
    printf("\n");
}
int main()
{
   strcpy(text, "GEEKSFORGEEKS$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "AAAAAAAAAA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ABCDEFG$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ABABABA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "ATCGATCGA$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "banana$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "abcpqrabpqpq$");
    buildsuffixtree();   
    repeat();
    freetree(root);
 
    strcpy(text, "pqrpqpqabab$");
    buildsuffixtree();   
    repeat();
    freetree(root);
}