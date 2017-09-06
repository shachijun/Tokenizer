//
//  tokenizer.c
//  Homework assing1
//
//  Created by Chijun Sha on 16/9/10.
//  Copyright © 2016 Chijun Sha. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
/*
 * Tokenizer type.  You need to fill in the type as part of your implementation.
 */
struct TokenizerT_ {
    char *string;
};
typedef struct TokenizerT_ TokenizerT;

/*
 * TKCreate creates a new TokenizerT object for a given token stream
 * (given as a string).
 *
 * TKCreate should copy the arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */
TokenizerT *TKCreate( char * ts ) {
    TokenizerT *t=(TokenizerT *)malloc(sizeof(TokenizerT));
    t->string=ts;
    return t;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */
void TKDestroy( TokenizerT * tk ) {
    if(tk!=NULL){
        free(tk);
        tk=NULL;
    }
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */
void escape(char*string,char c){
    char a;
    a = toascii(c);
    if (string[0]!='I') {
        printf("For %s character %c is an error:[%#2x]\n",string,c,a);
    }else{
        printf("%s character %c is an error:[%#2x]\n",string,c,a);
    }
    return;
}

int decimal(char *string,int i,int condition){
    if(i==strlen(string)){
        return condition;
    }else{
        if((string[i]>='0')&&(string[i]<='9')){
            return decimal(string, i+1,condition);
        }else{
            escape(string,string[i]);
            return decimal(string,i+1,-1);
        }
    }
}

int octal(char *string,int i,int condition){
    if(i==strlen(string)){
        return condition;
    }
    if((string[i]>='0')&&(string[i]<='7')){
        return octal(string, i+1,condition);
    }else{
        escape(string,string[i]);
        return octal(string, i+1,-1);
    }
}

int hexadecimal(char *string,int i,int condition){
    if(i==strlen(string)){
        return condition;
    }
    if((isxdigit(string[i]))||(string[i]>='a'&&string[i]<='f')||(string[i]>='A'&&string[i]<='F')){
        return hexadecimal(string, i+1,condition);
    }else{
        escape(string,string[i]);
        return hexadecimal(string,i+1, -1);
    }
}

int floating(char *string,int i,bool e,bool p,int condition){
    if(i==strlen(string)){
        if ((e==false)||(p==false)) {
            return condition;
        }else{
            if (condition==-1) {
                return condition;
            }else{
                return 0;
            }
        }
    }
    if(isdigit(string[i])){
        return floating(string, i+1,e,p,condition);
    }else if (string[i]=='.'){
        if((p==false)||(e==false)){
            escape(string,string[i]);
            return  floating(string, i+1,e,p,-1);
        }else{
            if (i<(strlen(string)-1)){
                i=i+1;
                if (isdigit(string[i])) {
                    return floating(string, i+1,e,false,condition);
                }else{
                    escape(string,string[i]);
                    return  floating(string, i+1,e,false,-1);
                }
            }
            escape(string,string[i]);
            return  floating(string, i+1,e,false,-1);
        }
    }else if((string[i]=='E')||(string[i]=='e')){
        if(e==false){
            escape(string,string[i]);
            return  floating(string, i+1,e,p,-1);
        }else{
            if (i>=(strlen(string)-2)) {
                escape(string,string[i]);
                return  floating(string, i+1,false,p,-1);
            }
            i=i+1;
            if (i<(strlen(string)-1)) {
                if((string[i]=='+')||(string[i]=='-')||isdigit(string[i])){
                    return  floating(string, i+1,false,p,condition);
                }else {
                    escape(string,string[i]);
                    return  floating(string, i+1,false,p,-1);
                }
            }else{
                escape(string,string[i]);
                return  floating(string, i+1,false,p,-1);
            }
        }
    }else{
        escape(string,string[i]);
        return  floating(string, i+1,e,p,-1);
    }
}
char *TKGetNextToken( TokenizerT * tk ) {
    char * string=tk->string;
    if(string!=NULL){
        if(isdigit(string[0])){
            if ((strlen(string)==1)&&(string[0]=='0')) {
                TKDestroy(tk);
                return "A zero: ";
            }
            if (strlen(string)>2) {
                if((string[0]=='0')&&((string[1]=='x')||(string[1]=='X'))){
                    if(hexadecimal(string,2,1)==1){
                        TKDestroy(tk);
                        return "A hexadecimal: ";
                    }
                    TKDestroy(tk);
                    return "Mal Formed: ";
                }
                int flo=floating(string, 0,true,true,1);
                if (flo==1){
                    TKDestroy(tk);
                    return "A floating: ";
                }else if(flo==-1){
                    TKDestroy(tk);
                    return "Mal Formed: ";
                }
            }
            if (string[0]!='0') {
                int dec=decimal(string,1,1);
                if (dec==-1) {
                    TKDestroy(tk);
                    return "Mal Formed: ";
                }else if(dec==1){
                    TKDestroy(tk);
                    return "A decimal: ";
                }
            }else{
                int oct=octal(string,0,1);
                if (oct==-1) {
                    TKDestroy(tk);
                    return "Mal Formed: ";
                }else if (oct==1){
                    TKDestroy(tk);
                    return "A octal: ";
                }
            }
        }else{
            int i;
            for (i=0;i<strlen(string);i++) {
                escape(string,string[i]);
            }
            TKDestroy(tk);
            return "Invalid input(start character is not a digit): ";
        }
    }
    TKDestroy(tk);
    return "Mal Formed: ";
}

/*
 * main will have a string argument (in argv[1]).
 * The string argument contains the tokens.
 * Print out the tokens in the second string in left-to-right order.
 * Each token should be printed on a separate line.
 */
char isinvalid(char i,bool ch,bool flo,bool plu,bool poin,bool bi){
    if (ch) {
        if (((i>='a')&&(i<='f'))||((i>='A')&&(i<='F'))||(isdigit(i))||(isspace(i))) {
            return '0';
        }else{
            return i;
        }
    }else if(flo){
        if (plu) {
            if ((isdigit(i))||(isspace(i))) {
                return '0';
            }else{
                return i;
            }
        }else{
            if ((isdigit(i)||(i=='+')||(i=='-'))||(isspace(i))){
                return '0';
            }else{
                return i;
            }
        }
    }else if(poin){
        if ((i=='e')||(i=='E')||(isdigit(i))||(isspace(i))) {
            return '0';
        }else{
            return i;
        }
    }else if (bi){
        if ((i=='.')||(i=='e')||(i=='E')||((i>='0')&&(i<='7'))||(isspace(i))) {
            return '0';
        }else{
            return i;
        }
    }else{
        if ((i=='.')||(i=='e')||(i=='E')||(isdigit(i))||(isspace(i))) {
            return '0';
        }else{
            return i;
        }
    }
}
int main(int argc, char **argv) {
    char *ts=argv[1];
    char *find;
    TokenizerT *t;
    int len=(int) strlen(ts);
    int c=0;
    int i;
    for(i=0;i<len;){
        if(isspace(ts[i])){
            i=i+1;
            c=i;
            continue;
        }
        bool ch=false,flo=false, plu=false,poin=false,bi=false,start=false;
        char inval=isinvalid(ts[i],ch,flo,plu,poin,bi);
        if ((isdigit(ts[i]))||(isspace(ts[i]))) {
            if (ts[i]=='0') {
                bi=true;
                if ((ts[i+1]=='x')||(ts[i+1]=='X')) {
                    ch=true;
                    i=i+2;
                }
                if (ts[i+1]!='.'){
                    start=true;
                }
            }
            while ((!isspace(ts[i]))&&(i<len)&&(inval=='0')){
                if(((ts[i]=='e')||(ts[i]=='E'))&&(ch==false)){
                    flo=true;
                }else if((ts[i]=='+')||(ts[i]=='-')){
                    plu=true;
                }else if(ts[i]=='.'){
                    if(start==true){
                        inval=ts[i];
                        break;
                    }
                    if (isspace(ts[i+1])) {
                        i=i+1;
                        break;
                    }
                    if (((i+1)==len)){
                        inval=ts[i];
                        break;
                    }
                    if (((i+1)<len)&&(!isdigit(ts[i+1]))){
                        i=i+1;
                        inval=ts[i];
                        break;
                    }
                    poin=true;
                }
                i=i+1;
                if (i<len) {
                    inval=isinvalid(ts[i],ch,flo,plu,poin,bi);
                }
            }
            if (c!=i) {
                int length=i-c;
                char str[length+1];
                int a;
                for (a=0; a<=length; a++) {
                    if (a==length) {
                        str[a]= '\0';
                        c++;
                    }else{
                        str[a]=ts[c];
                        c++;
                    }
                }
                t=TKCreate(str);
                find=TKGetNextToken(t);
                if(ts!=NULL){
                    printf("%s",find);
                    printf("%s\n",str);
                }
            }
        }else{
            inval=ts[i];
        }
        if (inval!='0') {
            if (isdigit(inval)) {
                c=i;
            }else{
                escape("Invalid Input,",inval);
                i=i+1;
                c=i;
            }
        }else{
            c=i;
        }
    }
    return 0;
}
