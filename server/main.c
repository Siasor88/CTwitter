#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define LISTEN_ERROR -1




//----------------------------------------------------------------------------------------------------
//functions Prototypes
char* AnalysingClientsCommand (char* clients_message);
int initialize (void);
int listening();
char* creatUser (char *user);
void creatUsersFiles(char* folderaddr,char*);
int if_username_exists(char *username);
char* login (char* login);
void tweet_count_pp (void);
char* creat_tweet(char* tweet,char * username);
void initialize_server(void);
char *token_generator();
char* analyzeCommand(char* clients_request);
char *refresh(char *username);
void refresh_reset();
char* like(char* like,char* username);
char *follow(char *follow,char* username);
long int refresh_initialize(char *username);
char *unfollow(char *unfollow,char* username);
char *logout(char *username);
char *changePassword(char *change_password, char *username);
char *time_right_now();
char *delete_tweet(char *delete_tweet,char *username);
char *advanced_search(char *advanced_search, char *username);
//----------------------------------------------------------------------------------------------------
//Variables
const int PORT=12345;
int tweet_count;
int ONLINE_USERS;
char *USER[100];
char *TOKEN[100];
//----------------------------------------------------------------------------------------------------
int main() {
    initialize_server();
    printf("\t Info | %s | Initialized\n", time_right_now());
    listening();
    //char * string = advanced_search("advancedSearch skbvfjmv, aaa","aaa");
    //printf("%s",string);
}

char *time_right_now(){
    time_t timer;
    static char buffer[26];
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y/%m/%d %H:%M:%S", tm_info);
    return buffer;
}

int listening() {
    int server_socket;
    while (1) {
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == INVALID_SOCKET) {
            puts("Socket Failed");
            continue;
        }
        int enable = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
            puts("setsockopt(SO_REUSEADDR) failed");
        }
        struct sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY; //Accept Anything
        server_addr.sin_port = htons(PORT);
        /*ADDR BINDING*/
        int bind_res = bind(server_socket, (struct sockaddr *) &server_addr, sizeof(server_addr));
        if (bind_res == SOCKET_ERROR) {
            puts("Bind Failed\n");
            continue;
        }
        int listen_resp = listen(server_socket,
                                 SOMAXCONN); // second arg is the number of requests that can stay in the queue.

        if (listen_resp == LISTEN_ERROR) {
            puts("Listen Failed\n");
            continue;
        } else { break; }
    }
    printf("\t Info | %s | Listening on port 12345\n", time_right_now());
    struct sockaddr_in client_addr;
    int len = sizeof(struct sockaddr_in);
    int client_socket;
    while ((client_socket=accept(server_socket, (struct sockaddr*)&client_addr, &len)) != 0) {
        char*respond;
        char buffer[1000];
        bzero(buffer,1000); //filling all the buffer with value = 0; (emptying buffer)
        recv(client_socket, buffer, 999, 0);
        if (buffer[strlen(buffer)-1] != '\n'){
            int sz = strlen(buffer);
            buffer[sz-1] = '\n';
            buffer[sz] = '\0';
        }
        printf("\t Info | %s | Request from /127.0.0.1: %s", time_right_now(), buffer);
        respond=analyzeCommand(buffer);
        printf("\t Info | %s | Response sent: %s\n", time_right_now(), respond);
        send(client_socket, respond, strlen(respond), 0);
        close(client_socket);
    }
}

void refresh_reset() {
    struct dirent *pDirent;
    char *address = malloc(sizeof (char)*300);
    strcpy(address, "Resources/Users/");
    DIR *pDir = opendir(address);
    while ((pDirent = readdir(pDir)) != NULL) {
        if (pDirent->d_type != DT_DIR || !strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,".."))
            continue;
        char *refresh_stat_addr = malloc(sizeof(char)*300);
        strcat(refresh_stat_addr, address);
        strcat(refresh_stat_addr, pDirent->d_name);
        strcat(refresh_stat_addr, "/refreshTime.txt");
        FILE *refresh_stat = malloc(sizeof(char)*10);
        refresh_stat=fopen(refresh_stat_addr,"w");
        fprintf(refresh_stat,"0");
        fclose(refresh_stat);
    }

}

char* creatUser (char *user) {
    static char server_respond[1000];
    bzero(server_respond,1000);
    char username[100];
    char password[100];
    char folderaddr[120];
    char signupStat[1000];
    sscanf(user, "%s%s%*c%[^\n]s", signupStat, username, password);
    int username_len = strlen(username);
    username[username_len - 1] = '\0';
    sprintf(folderaddr, "Resources/Users/%s", username);
    if(if_username_exists(username)){
        strcpy(server_respond,"{\"type\":\"Error\",\"message\":\"This username is already taken.\"}");
        return server_respond;
    } else {
        mkdir(folderaddr, 0777);
        creatUsersFiles(folderaddr, password);
        printf("\t Info | %s | User %s successfully registered.\n", time_right_now(), username);

        strcpy(server_respond,"{\"type\":\"Successful\",\"message\":\"\"}");
        return server_respond;
    }
}//done

void creatUsersFiles(char* folderaddr,char* Password){
    char *password=(char *)malloc(100);
    char *Bio =(char *)malloc(100);
    char *followings =(char *)malloc(100)  ;
    char *followers  =(char *)malloc(100)   ;
    char *tweets  =(char *)malloc(100)  ;
    char *numberOfFollowers=  (char *)malloc(100) ;
    char *numberOfFollowings  =(char *)malloc(100) ;
    char *loginStat  =(char *)malloc(100)  ;
    char *refreshTime = (char *)malloc(100);
    sprintf(password,"%s/password.txt",folderaddr);
    sprintf(Bio,"%s/bio.txt",folderaddr);
    sprintf(followings,"%s/followings",folderaddr);
    sprintf(followers,"%s/followers",folderaddr);
    sprintf(tweets,"%s/tweets.txt",folderaddr);
    sprintf(numberOfFollowers,"%s/numberOfFollowers.txt",folderaddr);
    sprintf(numberOfFollowings,"%s/numberOfFollowings.txt",folderaddr);
    sprintf(loginStat,"%s/loginStat.txt",folderaddr);
    sprintf(refreshTime,"%s/refreshTime.txt",folderaddr);
    FILE *passwordptr=fopen(password,"w");
    FILE *Bioptr=fopen(Bio,"w");
    mkdir(followings,0777);
    mkdir(followers,0777);
    FILE *tweetsptr=fopen(tweets,"w");
    FILE *loginStatptr=fopen(loginStat,"w");
    FILE *numberOfFollowersptr=fopen(numberOfFollowers,"w");
    FILE *numberOfFollowingsptr=fopen(numberOfFollowings,"w");
    FILE *refreshTimeptr=fopen(refreshTime,"w");
    fprintf(refreshTimeptr,"%d\n",0);
    fprintf(numberOfFollowersptr,"%d\n",0);
    fprintf(numberOfFollowingsptr,"%d\n",0);
    fprintf(passwordptr,"%s",Password);
    fprintf(loginStatptr,"0");
    //-------------------------------------------------------------------------
    free(Bio);
    free(followers);
    free(followings);
    free(numberOfFollowings);
    free(numberOfFollowers);
    free(password);
    free(loginStat);
    free(tweets);
    fclose(passwordptr);
    fclose(Bioptr);
    fclose(tweetsptr);
    fclose(loginStatptr);
    fclose(numberOfFollowersptr);
    fclose(numberOfFollowingsptr);
    fclose(refreshTimeptr);
}//done in bara ballaie ballaie nakhastam shologh she payen avordam

void initialize_server(void){
    FILE *info_ptr;
    int count;
    info_ptr=fopen("Resources/Info/info.txt","r");
    fscanf(info_ptr,"%d",&count);
    tweet_count=count;
    fclose(info_ptr);
    printf("\t Info | %s | Initializing...\n", time_right_now());
    struct dirent *pDirent;
    char *address = malloc(sizeof (char)*300);
    strcpy(address, "Resources/Users/");
    DIR *pDir = opendir(address);
    while ((pDirent = readdir(pDir)) != NULL) {
        if (pDirent->d_type != DT_DIR || !strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,".."))
            continue;
        char *login_stat_addr = malloc(sizeof(char)*300);
        strcat(login_stat_addr, address);
        strcat(login_stat_addr, pDirent->d_name);
        strcat(login_stat_addr, "/loginStat.txt");
        FILE *login_stat;

        login_stat=fopen(login_stat_addr,"w");
        fprintf(login_stat,"0");
        fclose(login_stat);

    }
    refresh_reset();

}

char *comment(char *comment,char* username){
    char comment_text[1000];
    int tweetId;
    char tweet_addr[100];
    char commentـaddr[100];
    FILE *comment_file;
    sscanf(comment,"%*s%*s%d%*c%[^\0]s",&tweetId,comment_text);
    if (tweetId < 0 || tweetId >= tweet_count){
        static char respond[100];
        printf("\t Info | %s | Tweet with this id is not available.\n", time_right_now());

        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Tweet with this id is not available.\"}\n");
        return respond;
    }
    sprintf(commentـaddr,"Resources/Tweets/%d/comments.txt",tweetId);
    comment_file=fopen(commentـaddr,"a");
    fprintf(comment_file,"\"%s\":\"%s\",",username,comment_text);
    fclose(comment_file);
    static char respond[100];
    printf("\t Info | %s | User %s successfully sent a comment.\n", time_right_now(), username);

    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"Comment is sent successfully.\"}\n");
    return respond;
}//done

char* login (char* login){
    char loginStat[1000];
    char username[1000];
    char password[1000];
    static char server_respond[1000];
    bzero(server_respond,1000);
    bzero(password,1000);
    sscanf(login,"%s %s %[^\n]s",loginStat,username,password);
    int username_len=strlen(username);
    username[username_len-1]='\0';
    //check user name exists or not
    char foldweaddr[1000];
    if (!if_username_exists(username)){
        strcpy(server_respond,"{\"type\":\"Error\",\"message\":\"The username you entered did not match our records. Please double-check and try again.\"}\n");
        return server_respond;
    } else{
        //find password
        char password_addr[1000];
        sprintf(password_addr,"Resources/Users/%s/password.txt",username);
        FILE *password_file;
        password_file=fopen(password_addr,"r");
        char real_pass[1000];
        fgets(real_pass,999,password_file);
        fclose(password_file);
        //check login stat
        char login_stat_addr [1000];
        sprintf(login_stat_addr,"Resources/Users/%s/loginStat.txt",username);
        FILE *login_stat;
        login_stat=fopen(login_stat_addr,"r");
        int log_stat;
        fscanf(login_stat,"%d",&log_stat);
        fclose(login_stat);

        if(log_stat==1){
            strcpy(server_respond,"{\"type\":\"Error\",\"message\":\"User is already logged in.\"}\n");
            return server_respond;
        }

        if (!strcmp(real_pass,password)){
            printf("\t Info | %s | User %s successfully logged in.\n", time_right_now(), username);
            login_stat=fopen(login_stat_addr,"w");
            fprintf(login_stat,"1");
            fclose(login_stat);
            strcpy(server_respond,"{\"type\":\"Token\",\"message\":\"");
            TOKEN[ONLINE_USERS]=(char *)malloc(50);
            TOKEN[ONLINE_USERS]=token_generator();
            USER[ONLINE_USERS]=(char *)malloc(100);
            strcpy(USER[ONLINE_USERS], username);
            strcat(server_respond,TOKEN[ONLINE_USERS]);
            strcat(server_respond,"\"}\n");
            ONLINE_USERS++;
            return server_respond;
        } else{
            strcpy(server_respond,"{\"type\":\"Error\",\"message\":\"The password you entered did not match to this username. Please double-check and try again.\"}\n");
            return server_respond;
        }

    }
} //done double checked

int if_username_exists(char *username){
    char folderaddr[100];
    sprintf(folderaddr, "Resources/Users/%s", username);
    DIR *folder_addr;
    folder_addr=opendir(folderaddr);

    if(folder_addr==NULL){
        return 0;
    } else{
        closedir(folder_addr);
        return 1;
    }

}//done

char* creat_tweet(char* tweet,char * username){
    char users_addr[100];
    char users_tweets_addr[100];
    char tweet_addr[100];
    char tweet_addr_like_count[100];
    char tweet_addr_comments[100];
    char tweet_addr_tweet[100];
    char tweet_content[1000];
    char tweet_time_addr[100];
    long int timeT;
    FILE *tweet_time;
    FILE *users_tweet_file;
    FILE *tweet_addr_file;
    FILE *likes;
    FILE *comments;
    sscanf(tweet,"%*s%*s%*s %[^\0]s",tweet_content);
    //creating tweets dir
    sprintf(users_addr,"Resources/Users/%s",username);
    strcpy(users_tweets_addr,users_addr);
    strcat(users_tweets_addr,"/tweets.txt");
    users_tweet_file=fopen(users_tweets_addr,"a");
    fprintf(users_tweet_file,"%d,",tweet_count);
    fclose(users_tweet_file);
    sprintf(tweet_addr,"Resources/Tweets/%d",tweet_count);
    mkdir(tweet_addr,0777);
    //creating files in tweet dir
    //tweet's txt
    strcpy(tweet_addr_tweet,tweet_addr);
    strcat(tweet_addr_tweet,"/tweet.txt");
    //comments txt
    strcpy(tweet_addr_comments,tweet_addr);
    strcat(tweet_addr_comments,"/comments.txt");
    //likes text
    strcpy(tweet_addr_like_count,tweet_addr);
    strcat(tweet_addr_like_count,"/likes.txt");
    //time text
    strcpy(tweet_time_addr,tweet_addr);
    strcat(tweet_time_addr,"/time.txt");
    //likes
    likes=fopen(tweet_addr_like_count,"w");
    fprintf(likes,"%d\n",0);
    fclose(likes);
    //comments
    comments=fopen(tweet_addr_comments,"w");
    fprintf(comments,"\"comments\":{");
    fclose(comments);

    tweet_addr_file=fopen(tweet_addr_tweet,"w");
    fprintf(tweet_addr_file,"{\"id\":%d,\"author\":\"%s\",\"content\":\"%s\",",tweet_count,username,tweet_content);
    fclose(tweet_addr_file);

    timeT=time(NULL);
    tweet_time=fopen(tweet_time_addr,"w");
    fprintf(tweet_time,"%ld\n",timeT);
    fclose(tweet_time);
    tweet_count_pp();
    static char respond[100];
    printf("\t Info | %s | User %s successfully sent a tweet.\n", time_right_now(), username);
    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"Tweet is sent successfully\"}\n");
    return respond;
}

char *follow(char *follow,char* username){
    char follower_addr[100];
    char following_addr[100];
    char followerNo_addr[100];
    char followingNo_addr[100];
    char who_is_going_to_be_followed[100];
    long int follow_time=time(NULL);
    FILE *follower_file;
    FILE *following_file;
    FILE *followersNo;
    FILE *followingsNo;
    int followers,followings;
    sscanf(follow,"%*s%*s%s",who_is_going_to_be_followed);
    sprintf(follower_addr,"Resources/Users/%s/followers/%s.txt",who_is_going_to_be_followed,username);
    sprintf(following_addr,"Resources/Users/%s/followings/%s.txt",username,who_is_going_to_be_followed);
    sprintf(followerNo_addr,"Resources/Users/%s/numberOfFollowers.txt",who_is_going_to_be_followed);
    sprintf(followingNo_addr,"Resources/Users/%s/numberOfFollowings.txt",username);
    //add  followings no
    followingsNo=fopen(followingNo_addr,"r");
    fscanf(followingsNo,"%d",&followings);
    followings++;
    fclose(followingsNo);
    followingsNo=fopen(followingNo_addr,"w");
    fprintf(followingsNo,"%d",followings);
    fclose(followingsNo);
    //add followers no
    followersNo=fopen(followerNo_addr,"r");
    fscanf(followersNo,"%d",&followers);
    followers++;
    fclose(followersNo);
    followersNo=fopen(followerNo_addr,"w");
    fprintf(followersNo,"%d",followers);
    fclose(followersNo);

    follower_file=fopen(follower_addr,"w");
    following_file=fopen(following_addr,"w");
    fprintf(follower_file,"%ld\n",follow_time);
    fprintf(following_file,"%ld\n",follow_time);
    fclose(follower_file);
    fclose(following_file);
    static char respond[100];
    sprintf(respond,"{\"type\":\"Successful\",\"message\":\"User %s is followed successfully.\"}\n", who_is_going_to_be_followed);
    printf("\t Info | %s | User %s successfully followed user %s\n", time_right_now(), username, who_is_going_to_be_followed);

    return respond;
}

void tweet_count_pp (void){
    FILE *info_ptr;
    int count;
    info_ptr=fopen("Resources/Info/info.txt","r");
    fscanf(info_ptr,"%d",&count);
    fclose(info_ptr);
    info_ptr=fopen("Resources/Info/info.txt","w");
    fprintf(info_ptr,"%d\n",count+1);
    fclose(info_ptr);
    ++tweet_count;
}

char *token_generator(){
    srand(time(0));
    char *token = malloc(sizeof (char)*50);
    for (int i = 0; i < 20; i++){
        int x = rand() % 62;
        if (x < 26) {
            char c = (char) (x + 'a');
            token[i] = c;
            token[i+1] = '\0';
        }
        else if (x < 52){
            char c = (char)(x-26+'A');
            token[i] = c;
            token[i+1] = '\0';
        }
        else{
            char c = (char)(x-52+'0');
            token[i] = c;
            token[i+1] = '\0';
        }
    }
    long int distinct = time(NULL);
    return token;
}

bool isprefix(char *pattern, char *text) {
    if (strlen(text) < strlen(pattern))
        return 0;
    for (int i = 0; i < strlen(pattern); i++)
        if (pattern[i] != text[i])
            return 0;
    return 1;
}

char *token_to_user(char *token){
    for (int i = 0; i < ONLINE_USERS; i++)
        if (!strcmp(TOKEN[i],token))
            return USER[i];
    return "Error_Token_User_Not_Found";
}

char *setbio(char* bio,char* username){
    char bio_stat[1000];
    char bio_addr[100];
    FILE *bio_file;
    sprintf(bio_addr,"Resources/Users/%s/bio.txt",username);
    sscanf(bio,"%*s%*s%*s%*c%[^\n]s",bio_stat);
    bio_file=fopen(bio_addr,"w");
    fprintf(bio_file,"\"bio\":\"%s\",",bio_stat);
    fclose(bio_file);
    static char respond[100];
    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"Bio is updated successfully.\"\n");

    printf("\t Info | %s | User %s's bio successfully updated.\n", time_right_now(), username);

    return respond;
}

long int refresh_initialize(char *username){
    char username_addr[100];
    FILE *refresh_file;
    long int refresh_time;
    long int new_refresh_time;
    sprintf(username_addr,"Resources/Users/%s/refreshTime.txt",username);
    refresh_file=fopen(username_addr,"r");
    fscanf(refresh_file,"%ld",&refresh_time);
    fclose(refresh_file);
    new_refresh_time=time(NULL);
    refresh_file=fopen(username_addr,"w");
    fprintf(refresh_file,"%ld\n",new_refresh_time);
    fclose(refresh_file);
    return refresh_time;
}

char* like(char* like,char* username){
    char tweet_time_addr[100];
    char tweet_like_addr[100];
    int tweet_id;
    int count_like;
    sscanf(like,"%*s%*s%d",&tweet_id);
    if (tweet_id >= tweet_count || tweet_id < 0){
        static char respond[100];
        printf("\t Info | %s | Tweet with this id is not available.\n", time_right_now());
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Tweet with this id is not available\"}\n");
        return respond;
    }
    sprintf(tweet_like_addr,"Resources/Tweets/%d/likes.txt",tweet_id);
    sprintf(tweet_time_addr,"Resources/Tweets/%d/time.txt",tweet_id);
    long int timet=time(NULL);
    FILE *time_file;
    time_file=fopen(tweet_time_addr,"w");
    fprintf(time_file,"%ld\n",timet);
    fclose(time_file);
    FILE *like_file;
    like_file=fopen(tweet_like_addr,"r");
    fscanf(like_file,"%d",&count_like);
    fclose(like_file);
    like_file=fopen(tweet_like_addr,"w");
    fprintf(like_file,"%d\n",count_like+1);
    fclose(like_file);
    static char respond[100];
    printf("\t Info | %s | User %s successfully liked tweet %d.\n", time_right_now(), username, tweet_id);
    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"Tweet has Successfully liked.\"}\n");
    return respond;
}//done

char *unfollow(char *unfollow,char* username){
    static char respond[100];
    char unfollower_addr[100];
    char unfollowing_addr[100];
    char who_is_going_to_be_unfollow[100];
    char followerNo_addr[100];
    char followingNo_addr[100];
    long int follow_time=time(NULL);
    FILE *follower_file;
    FILE *following_file;
    FILE *followersNo;
    FILE *followingsNo;
    int followers,followings;

    sscanf(unfollow,"%*s%*s%s",who_is_going_to_be_unfollow);
    sprintf(unfollower_addr,"Resources/Users/%s/followers/%s.txt",who_is_going_to_be_unfollow,username);
    sprintf(unfollowing_addr,"Resources/Users/%s/followings/%s.txt",username,who_is_going_to_be_unfollow);
    remove(unfollowing_addr);
    remove(unfollower_addr);
    //reduce  followings no

    sprintf(followerNo_addr, "Resources/Users/%s/numberOfFollowers.txt", who_is_going_to_be_unfollow);
    sprintf(followingNo_addr, "Resources/Users/%s/numberOfFollowings.txt", username);
    followingsNo = fopen(followingNo_addr, "r");
    fscanf(followingsNo, "%d", &followings);
    followings--;
    fclose(followingsNo);
    followingsNo = fopen(followingNo_addr, "w");
    fprintf(followingsNo, "%d", followings);
    fclose(followingsNo);
    //reduce followers no
    followersNo = fopen(followerNo_addr, "r");
    fscanf(followersNo, "%d", &followers);
    followers--;
    fclose(followersNo);
    followersNo = fopen(followerNo_addr, "w");
    fprintf(followersNo, "%d", followers);
    fclose(followersNo);
    strcpy(respond, "{\"type\":\"Successful\",\"message\":\"User UnFollowed Successfully.\"}\n");
    return respond;

}

char *show_tweets(char *tweet_id){
    char *respond = (char*)malloc(sizeof(char)*1000);
    char *addr = (char*)malloc(sizeof(char)*100);
    char *comments = (char*)malloc(sizeof(char)*1000);

    bzero(respond,1000);
    bzero(addr,100);

    sprintf(addr, "Resources/Tweets/%s/tweet.txt", tweet_id);
    FILE *tweet_file;
    tweet_file = fopen(addr, "r");
    fscanf(tweet_file, "%[^\0]s", respond);
    fclose(tweet_file);

    sprintf(addr, "Resources/Tweets/%s/comments.txt", tweet_id);
    FILE *comment_file;
    comment_file = fopen(addr, "r");
    fscanf(comment_file, "%[^EOF]s", comments);
    fclose(comment_file);

    if (comments[strlen(comments)-1] == ',')
        comments[strlen(comments)-1] = '\0';
    int sz = strlen(comments);
    comments[sz] = '}';
    comments[sz+1] = ',';
    comments[sz+2] = '\0';

    strcat(respond, comments);

    char *Nlikes = malloc(sizeof(char)*100);
    sprintf(addr, "Resources/Tweets/%s/likes.txt", tweet_id);
    FILE *likes_file;
    likes_file = fopen(addr, "r");
    fscanf(likes_file, "%s", Nlikes);
    fclose(likes_file);

    strcat(respond, "\"likes\":");
    strcat(respond, Nlikes);
    strcat(respond, "}");

    return respond;
}

long int get_tweet_time(char *tweet_id){
    char addr[100];
    sprintf(addr, "Resources/Tweets/%s/time.txt", tweet_id);
    FILE *tweet_time_file;
    tweet_time_file = fopen(addr, "r");
    long int tweet_time;
    fscanf(tweet_time_file, "%ld", &tweet_time);
    fclose(tweet_time_file);
    return tweet_time;
}

char *show_user_tweets(char *username, long int time){
    FILE *tweets;
    static char addr[100];
    char *str;
    char *tweet_id;
    str = malloc(sizeof(char)*1000);
    sprintf(addr,"Resources/Users/%s/tweets.txt", username);
    tweets=fopen(addr,"r");
    fscanf(tweets, "%[^\n]s", str);
    fclose(tweets);

    tweet_id = (char*)malloc(sizeof(char)*10);
    bzero(tweet_id,10);

    int counter = 0;
    char *tweets_array=(char *)malloc(1000);
    tweets_array[0] = '\0';
    bzero(tweets_array,1000);
    for (int i = 0; i < strlen(str); i++){
        if (str[i] == ','){
            if (get_tweet_time(tweet_id) > time) {
                if (counter > 0)
                    strcat(tweets_array, ",");
                strcat(tweets_array, show_tweets(tweet_id));
                counter++;
            }
            tweet_id[0] = '\0';
            continue;
        }
        strncat(tweet_id, &str[i], 1);
    }
    return tweets_array;
}

int cmpfunc (const void * a, const void * b) {
    return ( *(int*)b - *(int*)a );
}

char *refresh(char *username){
    long int last_refresh_time = refresh_initialize(username);
    struct dirent *pDirent;
    char address[300];
    sprintf(address, "Resources/Users/%s/followers", username);
    DIR *pDir = opendir(address);
    char *user[100];
    long int timeFollow[100];
    int count_of_users = 0;

    while ((pDirent = readdir(pDir)) != NULL){
        if (!strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,".."))
            continue;
        char user_addr[300];
        memset(user_addr, 0, sizeof(user_addr));
        strcat(user_addr, address);
        strcat(user_addr, "/");
        strcat(user_addr, pDirent->d_name);

        FILE *user_time;
        user_time=fopen(user_addr,"r");
        fscanf(user_time, "%ld", &timeFollow[count_of_users]);
        fclose(user_time);

        char *username = pDirent->d_name;
        username[strlen(username)-4] = '\0';
        user[count_of_users++] = username;
    }

    sprintf(address, "Resources/Users/%s/followings", username);
    pDir = opendir(address);

    int count_of_followers = count_of_users;

    while ((pDirent = readdir(pDir)) != NULL){
        if (!strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,".."))
            continue;

        char user_addr[300];
        memset(user_addr, 0, sizeof(user_addr));

        strcat(user_addr, address);
        strcat(user_addr, "/");
        strcat(user_addr, pDirent->d_name);

        char *username = pDirent->d_name;
        username[strlen(username)-4] = '\0';

        long int temporary_time;

        FILE *user_time;
        user_time=fopen(user_addr,"r");
        fscanf(user_time, "%ld", &temporary_time);
        fclose(user_time);

        bool firstTime = 0;
        for (int i = 0; i < count_of_followers; i++)
            if (!strcmp(username,user[i]))
                firstTime = 1;

        if (firstTime == 0) {
            user[count_of_users] = username;
            timeFollow[count_of_users++] = temporary_time;
        }
    }


    int *tweet_list = (int*)malloc(sizeof(int)*100);
    int cnt = 0;

    for (int i = 0; i < count_of_users; i++){
        FILE *tweets;
        static char addr[100];
        char *str;
        str = malloc(sizeof(char)*1000);
        sprintf(addr,"Resources/Users/%s/tweets.txt", user[i]);
        tweets=fopen(addr,"r");
        fscanf(tweets, "%[^\0]s", str);
        fclose(tweets);

        char *tweet_id = (char*)malloc(sizeof(char)*10);
        bzero(tweet_id,10);

        for (int j = 0; j < strlen(str); j++){
            if (str[j] == ','){
                if (timeFollow[i] > last_refresh_time || get_tweet_time(tweet_id) > last_refresh_time)
                    tweet_list[cnt++] = atoi(tweet_id);
                tweet_id[0] = '\0';
                continue;
            }
            strncat(tweet_id, &str[j], 1);
        }
        free(tweet_id);
    }

    qsort(tweet_list, cnt, sizeof(int), cmpfunc);

    static char respond[5000];
    strcpy(respond, "{\"type\":\"List\",\"message\":[");

    for (int i = 0; i < cnt; i++){
        char *tweet_id = (char*)malloc(sizeof(char)*10);
        sprintf(tweet_id, "%d", tweet_list[i]);
        if (i > 0)
            strcat(respond,",");
        strcat(respond,show_tweets(tweet_id));
    }

    if (cnt > 0) {
        respond[strlen(respond) - 1] = '\0';
        strcat(respond, "}]}");
    }
    else {
        strcat(respond, "]}");
    }
    printf("\t Info | %s | User %s successfully refreshed timeline.\n", time_right_now(), username);

    return respond;
}

char *get_user_bio(char *username){
    char *addr = malloc(sizeof(char)*100);
    sprintf(addr, "Resources/Users/%s/bio.txt", username);
    char *bio_text = malloc(sizeof (char)*1000);

    FILE *bio;
    bio=fopen(addr, "r");
    fscanf(bio, "%[^\0]s", bio_text);
    fclose(bio);
    if (strlen(bio_text) < 4){
        strcpy(bio_text, "\"bio\":\"\",");
    }
    return bio_text;
}

char *follow_status(char *me, char *that){
    if (!strcmp(me,that))
        return "Yourself";
    char *addr = malloc(sizeof(char)*100);
    sprintf(addr, "Resources/Users/%s/followings/%s.txt", me, that);
    if (access(addr, F_OK) == 0)
        return "Followed";
    return "NotFollowed";
}

char *show_profile(char *username, char *token){
    char *respond = malloc(sizeof (char)*1000);
    sprintf(respond, "{\"type\":\"Profile\",\"message\":{");

    char *username_message = malloc((sizeof (char)*100));
    sprintf(username_message, "\"username\":\"%s\",", username);
    strcat(respond, username_message);
    free(username_message);

    strcat(respond, get_user_bio(username));

    char *Nfollowers_message = malloc(sizeof(char)*100);
    char *count_of_followers = malloc(sizeof(char)*10);
    char *addr = malloc(sizeof(char)*100);
    sprintf(addr, "Resources/Users/%s/numberOfFollowers.txt", username);
    FILE *user_cof;
    user_cof=fopen(addr, "r");
    fscanf(user_cof, "%s", count_of_followers);
    sprintf(Nfollowers_message, "\"numberOfFollowers\":%s,", count_of_followers);
    strcat(respond, Nfollowers_message);
    free(Nfollowers_message);
    free(count_of_followers);

    char *Nfollowings_message = malloc(sizeof(char)*100);
    char *count_of_followings = malloc(sizeof(char)*10);
    sprintf(addr, "Resources/Users/%s/numberOfFollowings.txt", username);
    user_cof=fopen(addr, "r");
    fscanf(user_cof, "%s", count_of_followings);
    sprintf(Nfollowings_message, "\"numberOfFollowings\":%s,", count_of_followings);
    strcat(respond, Nfollowings_message);
    free(Nfollowings_message);
    free(count_of_followings);

    char *followStatus_message = malloc(sizeof(char)*100);
    sprintf(followStatus_message, "\"followStatus\":\"%s\",", follow_status(token_to_user(token),username));
    strcat(respond, followStatus_message);
    free(followStatus_message);

    char *all_tweets = malloc(sizeof(char)*1000);
    bzero(all_tweets,1000);
    sprintf(all_tweets, "\"allTweets\":[");
    strcat(all_tweets, show_user_tweets(username, -1));
    strcat(all_tweets, "]");
    strcat(respond, all_tweets);
    free(all_tweets);
    strcat(respond, "}}\n");

    printf("\t Info | %s | User %s successfully viewed his/her profile.\n", time_right_now(), username);

    return respond;
}

char *search(char *search,char *username){
    char username_we_are_searching_for[100];
    char *address=(char *)malloc(100);
    static char respond[1000];
    char *respond_;
    char *token = malloc(sizeof(char)*50);
    sscanf(search,"%*s%s%*c%[^\n]s", token, username_we_are_searching_for);
    if(!strcmp(username,username_we_are_searching_for)){
        printf("\t Info | %s | Searched username is your username.\n", time_right_now(), username);

        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Searched username is your username.\"}\n");
        return respond;
    }
    sprintf(address,"Resources/Users/%s", username_we_are_searching_for);

    DIR *user_file;
    user_file=opendir(address);
    if(user_file==NULL){
        printf("\t Info | %s | This username is not available.\n", time_right_now());

        strcpy(respond,"{\"type\":\"Error\",\"message\":\"This username is not available.\"}\n");
        return respond;
    }
    respond_=show_profile(username_we_are_searching_for, token);
    closedir(user_file);
    printf("\t Info | %s | User %s successfully searched user %s.\n", time_right_now(), username, username_we_are_searching_for);

    return respond_;
}

char *logout(char *username){

    bool isCorrect = 0;
    for (int i = 0; i < ONLINE_USERS; i++){
        if (!strcmp(username, USER[i])){
            isCorrect = 1;
            USER[i] = USER[ONLINE_USERS-1];
            TOKEN[i] = TOKEN[ONLINE_USERS-1];
            ONLINE_USERS--;
            break;
        }
    }

    if (!isCorrect){
        static char respond[100];
        printf("Info | %s | User %s was not logged in.\n", time_right_now(), username);
        sprintf(respond,"{\"type\":\"Error\",\"message\":\"User %s was not logged in.\"}\n", username);
        return respond;
    }
    char logoutStat_addr[100];
    static char respond[100];
    FILE *logoutStat_file;
    sprintf(logoutStat_addr,"Resources/Users/%s/loginStat.txt",username);
    logoutStat_file=fopen(logoutStat_addr,"w");
    fprintf(logoutStat_file,"0");
    fclose(logoutStat_file);
    printf("\t Info | %s | User %s successfully logged out.\n", time_right_now(), username);
    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"User Logged Out Successfully.\"}\n");
    return respond;
} // Doneee

char* analyzeCommand(char* clients_request){
    if (isprefix("login", clients_request)) {
        char type[100];
        char username[100];
        char password[100];
        sscanf(clients_request, "%s%s%s", type, username, password);
        username[strlen(username)-1] = '\0';

        return login(clients_request);
    }
    if (isprefix("signup", clients_request)){
        char type[100];
        char username[100];
        char password[100];
        sscanf(clients_request, "%s%s%s", type, username, password);
        username[strlen(username)-1] = '\0';
        return creatUser(clients_request);
    }
    if (isprefix("profile", clients_request)){
        char token[100];
        sscanf(clients_request, "%*s%s", token);
        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found")) {
            printf("Info | %s | Token is invalid.\n", time_right_now());
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        }
        return show_profile(user_request, token);
    }
    if (isprefix("logout", clients_request)){
        char type[100];
        char user_token[100];
        char *username=(char *)malloc(100* sizeof(char));
        sscanf(clients_request, "%s%s", type, user_token);
        username=token_to_user(user_token);
        if (!strcmp(username,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        return logout(username);
    }
    if (isprefix("set bio", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';
        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found")) {
            printf("\t\033[0;31m Info | %s |\033[0m Token is invalid.\n", time_right_now());
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        }

        return setbio(clients_request, user_request);
    }

    if (isprefix("change password", clients_request)){ // TODO
        char user_token[100];
        sscanf(clients_request, "%*s%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';
        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found")) {
            printf("Info | %s | Token is invalid.\n", time_right_now());
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        }
        return changePassword(clients_request,user_request);
    }

    if (isprefix("search", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';
        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        return search(clients_request, token_to_user(user_token));
    }
    if (isprefix("follow", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';
        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        return follow(clients_request, token_to_user(user_token));
    }
    if (isprefix("unfollow", clients_request)) {
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';

        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);
        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        return unfollow(clients_request, token_to_user(user_token));
    }
    if (isprefix("send tweet ", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';

        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);

        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";

        return creat_tweet(clients_request, token_to_user(user_token));
    }
    if (isprefix("refresh", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);

        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);

        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";

        return refresh(user_request);
    }
    if (isprefix("like ", clients_request)){
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);
        user_token[strlen(user_token)-1] = '\0';

        char *user_request = malloc(sizeof(char)*100);
        user_request = token_to_user(user_token);

        if (!strcmp(user_request,"Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";

        return like(clients_request, user_request);
    }
    if (isprefix("comment ", clients_request)) {
        char user_token[100];
        sscanf(clients_request, "%*s%s", user_token);
        user_token[strlen(user_token) - 1] = '\0';

        char *user_request = malloc(sizeof(char) * 100);
        user_request = token_to_user(user_token);

        if (!strcmp(user_request, "Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";

        return comment(clients_request, user_request);
    }
    if(isprefix("delete",clients_request)){
        char user_token[100];
        sscanf(clients_request,"%*s%s",user_token);
        user_token[strlen(user_token)-1]='\0';

        char *user_request = malloc(sizeof(char) * 100);
        user_request = token_to_user(user_token);

        if (!strcmp(user_request, "Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";

        return delete_tweet(clients_request, user_request);
    }
    if (isprefix("advancedSearch",clients_request)){
        char *username=(char *)malloc(100);
        char pattern[100];
        char user_token[100];
        sscanf(clients_request,"%*s %s %s",user_token,pattern);
        int token_len=(int)strlen(user_token);
        user_token[token_len-1]='\0';
        username=token_to_user(user_token);
        if (!strcmp(username, "Error_Token_User_Not_Found"))
            return "{\"type\":\"Error\",\"message\":\"Token is invalid.\"}";
        return advanced_search(clients_request,username);
    }
    return "{\"type\":\"Error\",\"message\":\"Bad request format.\"}";
}

char *changePassword(char *change_password, char *username){
    //find password
    char password_addr[100];
    sprintf(password_addr,"Resources/Users/%s/password.txt",username);
    FILE *password_file;
    password_file=fopen(password_addr,"r");
    char real_pass[1000];
    fgets(real_pass,999,password_file);
    fclose(password_file);
    static char respond[100];
    int oldpass_len;
    char oldpass[1000];
    char newpass[1000];
    sscanf(change_password,"%*s %*s %*s %s %s",oldpass,newpass);
    oldpass_len=(int)strlen(oldpass);
    oldpass[oldpass_len-1]='\0';
    if (strcmp(oldpass,real_pass)){
        printf("\t Info | %s | Entered current password is wrong.\n", time_right_now());
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Entered current password is wrong.\"}\n");
        return respond;
    }
    if (!strcmp(oldpass, newpass)){
        printf("\t Info | %s | New password is equal to current password.\n", time_right_now());
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"New password is equal to current password.\"}\n");
        return respond;
    }
    if (strlen(newpass) > 16){
        printf("\t Info | %s | Password length exceed from 16.\n", time_right_now());
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Password length exceed from 16.\"}\n");
        return respond;
    }
    password_file=fopen(password_addr,"w");
    fprintf(password_file,"%s",newpass);
    fclose(password_file);
    strcpy(respond,"{\"type\":\"Successful\",\"message\":\"Password is changed successfully.\"}\n");

    printf("\t Info | %s | User %s's password successfully changed.\n", time_right_now(), username);

    return respond;
}

char *delete_tweet(char *delete_tweet,char *username){
    static char respond[100];
    int tweet_id;
    char tweet_list[1000];
    char tweet_list_addr[100];
    char tweet_addr[100];
    char like_addr[100];
    char comments_addr[100];
    char time_addr[100];
    char tweet_dir_addr[100];
    char tweet[1000];
    char author[1000];
    char tweet_folder_addr[100];
    FILE *tweet_author_file;
    FILE *tweet_list_file;
    sscanf(delete_tweet,"%*s %*s %d",&tweet_id);
    //creating addr's
    sprintf(author,"{\"id\":%d,\"author\":\"%s\"",tweet_id,username);
    sprintf(tweet_addr,"Resources/Tweets/%d/tweet.txt",tweet_id);
    sprintf(tweet_dir_addr,"Resources/Tweets/%d",tweet_id);
    sprintf(like_addr,"Resources/Tweets/%d/likes.txt",tweet_id);
    sprintf(comments_addr,"Resources/Tweets/%d/comments.txt",tweet_id);
    sprintf(time_addr,"Resources/Tweets/%d/time.txt",tweet_id);
    sprintf(tweet_list_addr,"Resources/Users/%s/tweets.txt",username);
    //checking that we can remove that tweet or not
    tweet_author_file=fopen(tweet_addr,"r");
    if (tweet_author_file==NULL){
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"Tweet Doesn\'t Exist.\"}\n");
        return respond;
    }
    fscanf(tweet_author_file,"%[^EOF]s",tweet);
    if (!isprefix(author,tweet)){
        strcpy(respond,"{\"type\":\"Error\",\"message\":\"This tweet doesn\'t belong to you.\"}\n");
        fclose(tweet_author_file);
        return respond;
    }
    fclose(tweet_author_file);
    //deleting tweets files
    remove(like_addr);
    remove(comments_addr);
    remove(tweet_addr);
    remove(time_addr);
    rmdir(tweet_dir_addr);
    // removing tweet from authors tweetlist
    tweet_list_file=fopen(tweet_list_addr,"r");
    fscanf(tweet_list_file,"%[^EOF]s",tweet_list);
    fclose(tweet_list_file);
    printf("%s\n",tweet_list);
    tweet_list_file=fopen(tweet_list_addr,"w");
    fclose(tweet_list_file);
    int ptr=0;
    int tweet_list_len=strlen(tweet_list);
    while (1){
        int n;
        char flag;
        if(ptr>=tweet_list_len) {
            break;
        }
        sscanf(tweet_list+ptr,"%d%*c",&n);
        printf("%d\n",n);
        if(n==tweet_id){
            int N=n;
            if (N==0){
                ptr++;
            }
            while (N!=0){
                ptr++;
                N/=10;
            }
            ptr++;
            continue;
        } else{
            tweet_list_file=fopen(tweet_list_addr,"a");
            fprintf(tweet_list_file,"%d,",n);
            fclose(tweet_list_file);
            int N=n;
            if (N==0){
                ptr++;
            }
            while (N!=0) {
                ptr++;
                N /= 10;
            }
            ptr++;
        }
    }
    strcpy( respond,"{\"type\":\"Successful\",\"message\":\"Tweet Has been Deleted Successfully.\"}");
    return respond;
}

char *advanced_search(char *advanced_search, char *username){
    static char respond[1000];
    strcpy(respond,"Usernames :\n");
    char pattern[100];
    sscanf(advanced_search,"%*s %*s %s",pattern);
    static char *users[101];
    for (int i = 1; i <=100 ; ++i) {
        users[i]=(char *)malloc(100);
    }

    struct dirent *pDirent;
    char *address = malloc(sizeof (char)*300);
    strcpy(address, "Resources/Users/");
    DIR *pDir = opendir(address);
    while ((pDirent = readdir(pDir)) != NULL) {
        if (pDirent->d_type != DT_DIR || !strcmp(pDirent->d_name,".") || !strcmp(pDirent->d_name,".."))
            continue;
        char *flag;
        char *this_user;
        this_user=pDirent->d_name;
        flag=strstr(pDirent->d_name,pattern);
        if(flag==NULL){
            continue;
        } else{
            if (!strcmp(flag,username)){
                continue;
            }
           strcat(respond,this_user);
           strcat(respond,"\n");
        }
    }
    if(!strcmp(respond,"Usernames :\n")){
        strcpy(respond,"There is No Usernames Containing This Pattern.\n");
        return respond;
    }
    return respond;
}

