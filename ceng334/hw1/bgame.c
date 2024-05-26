#include "logging.h"
#include "message.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <poll.h>

#include <string.h>
#include <stdlib.h>

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)



typedef struct bomber_data {
	coordinate position;
	char* exec_path;
	int arg_count;
	char** args;
	pid_t pid;
	int alive;
}bomber_d;

typedef struct map_data{
// 0 means no object 1 means obstacle 2 means bomber 3 means bomb 4 means both bomb and bomber
	int obj_type;
	int player;
	pid_t bomb;
	int durability;

}map_data;

typedef struct bomb_d{
	coordinate position;
	unsigned int radius;
	pid_t pid;
	char** args;
}bom;


int main(){

	int height, width, obstacle_count, bomber_count, bomb_count=0, game_over=0, digit = 0, damage=0, alive_bomber=0,remaining_bombs=0, radius=0; //dont forget 2d array access is reversed
	int x, y, durability, arg_count, bomberx, bombery, bombx, bomby;
	scanf("%d %d %d %d", &width, &height, &obstacle_count, &bomber_count);	
	alive_bomber = bomber_count;
    map_data** map = (map_data **) malloc(height * sizeof(map_data *)); 

    for (int i = 0; i < height; i++) {
        map[i] = (map_data *) malloc(width * sizeof(map_data)); // Allocate each row
    }
	
	for(int i=0;i<obstacle_count;i++){ //parse the object inputs
		scanf("%d %d %d", &x, &y, &durability);
		map[y][x].obj_type = 1;
		map[y][x].durability = durability;

	}
	
	bomber_d* bombers = (bomber_d*) malloc(bomber_count*sizeof(bomber_d));

	for(int i=0;i< bomber_count;i++){
		scanf(" %d %d %d", &x, &y, &arg_count);
		bombers[i].position.x= x;
        bombers[i].position.y = y;
		bombers[i].arg_count= arg_count;
		bombers[i].alive = 1;
		map[y][x].obj_type  = 2;
		
		

		scanf(" %ms", &bombers[i].exec_path);
		
		
		bombers[i].args = (char**) malloc((arg_count+1)*sizeof(char*));

		bombers[i].args[0]=bombers[i].exec_path;

		for(int j=1; j<arg_count; j++){		//parse the arguments
		
			scanf(" %ms",&bombers[i].args[j]);

		}
		
		bombers[i].args[arg_count] = NULL;


	}

	//fd[bomber_count][2]
	int** fd= (int**) malloc(bomber_count*sizeof(int*));
	pid_t* pids = malloc(bomber_count * sizeof(pid_t));
	//pid_t pids[bomber_count]; //pids of the child processes

	for(int i=0;i<bomber_count;i++){ //create the pipes for each bomber
		fd[i] = (int*) malloc(2*sizeof(int));
		
		if (PIPE(fd[i])<0){
			perror("pipe error");		
			exit(1);	
		}
		
		if((pids[i] = fork())<0){
			perror("fork error");
			exit(1);
		}
		
		else if(pids[i]==0){ //child
			close(fd[i][1]); //child uses fd[0] for r&w
			
			dup2(fd[i][0], 0);
			dup2(fd[i][0], 1);

			//close();
			execvp(bombers[i].exec_path, bombers[i].args); //execute the bomber processes
		
		}



		else{ //parent
			map[bombers[i].position.y][bombers[i].position.x].player = i; //which player
			bombers[i].pid = pids[i];
			close(fd[i][0]); //parent(controller) uses fd[1] for r&w
		
		}

	}

	bom* bombs = malloc(sizeof(bom));
	int** bomb_fd = malloc(sizeof(int*));
	pid_t* bomb_pids = malloc(sizeof(pid_t));

	int nfds = bomber_count;
	struct pollfd* fds = malloc(nfds* sizeof(struct pollfd));
	//struct pollfd fds[nfds];

	od see_objects[25];
	unsigned int see=0;
	
	obsd* damaged_obstacles = (obsd*) malloc(sizeof(obsd)); //for the explosion print

	while(!game_over || remaining_bombs>0){ //main loop while there are bombers or remaining bombs
		nfds = bomb_count + bomber_count;
		fds = realloc(fds, nfds*sizeof(struct pollfd));
		for(int i=0;i<bomb_count;i++){ //set up poll for bomb
			fds[i].fd = bomb_fd[i][1];
			fds[i].revents = 0;
			fds[i].events = POLLIN;
		}

		for(int i = bomb_count, j=0;i<nfds;i++, j++){ // set up poll for bomber
			fds[i].fd = fd[j][1]; 
			fds[i].revents = 0;
			fds[i].events = POLLIN;
		}

		if(poll(fds, nfds, 0)==-1){
			perror("poll error");
			exit(1);
		}

		//printf("BOMB COUNT: %d ", bomb_count);	
		for(int i=0, j=0;i<bomb_count;i++, j++){
			if(bomb_pids[j]==-1)continue;
			if(fds[i].revents & POLLIN){
				
				im* message = (im*) malloc(sizeof(im));

				ssize_t bytes_read = read_data(fds[i].fd, message);
				
				if(bytes_read<0){
					perror("bomb read error");
					exit(1);
				}

					imp* imp_message = (imp*) malloc(sizeof(imp));	

					imp_message->m = message;
        			imp_message->pid = bomb_pids[j];
        			print_output(imp_message, NULL, NULL, NULL);
					
					damage = 0;
					//bombs[j].
					remaining_bombs--;
					bombx = bombs[j].position.x;
					bomby = bombs[j].position.y;
					radius = bombs[j].radius;						
						//map[y][x-3]  map[y][x+3] and map[y-3][x] map[y+3][x]

					for(int b = bombx-radius;b<bombx;b++){ //for horizontal
						if(b>=0 && b<width && map[bomby][b].obj_type != 0){
							if(map[bomby][b].obj_type == 1){ //if obstacle
								damage++;
								damaged_obstacles = realloc(damaged_obstacles, damage * sizeof(obsd));
								damaged_obstacles[damage-1].position.x = b;
								damaged_obstacles[damage-1].position.y = bomby;
								if(map[bomby][b].durability != -1){
									map[bomby][b].durability--; //report this later
									if(map[bomby][b].durability == 0) map[bomby][b].obj_type = 0;
								}
								damaged_obstacles[damage-1].remaining_durability = map[bomby][b].durability;
                                break;
                           }

							if(map[bomby][b].obj_type == 2 || map[bomby][b].obj_type == 4){
									bombers[map[bomby][b].player].alive = -1;
									if(map[bomby][b].obj_type == 2) map[bomby][b].obj_type = 0;
									else map[bomby][b].obj_type = 3;	
							}
						}
					}
						
					for(int b = bombx;b<=bombx+radius;b++){ //for horizontal
						if(b>=0 && b<width && map[bomby][b].obj_type != 0){
							if(map[bomby][b].obj_type == 1){ //if obstacle
								damage++;
								damaged_obstacles = realloc(damaged_obstacles, damage * sizeof(obsd));
								damaged_obstacles[damage-1].position.x = b;
								damaged_obstacles[damage-1].position.y = bomby;
								if(map[bomby][b].durability != -1){
									map[bomby][b].durability--; //report this later
									if(map[bomby][b].durability == 0) map[bomby][b].obj_type = 0;
								}
								damaged_obstacles[damage-1].remaining_durability = map[bomby][b].durability;
                                    break;
                            }

							if(map[bomby][b].obj_type == 2 || map[bomby][b].obj_type == 4){
								
									bombers[map[bomby][b].player].alive = -1;
									if(map[bomby][b].obj_type == 2) map[bomby][b].obj_type = 0;
									else map[bomby][b].obj_type = 3;
									
							}
						}
                    }


					for(int b = bomby-radius;b<bomby;b++){ //for vertical
						if(b>=0 && b<height && map[b][bombx].obj_type != 0){
                            if(map[b][bombx].obj_type == 1){
								damage++;
								damaged_obstacles = realloc(damaged_obstacles, damage * sizeof(obsd));
								damaged_obstacles[damage-1].position.x = bombx;
								damaged_obstacles[damage-1].position.y = b;
								if(map[b][bombx].durability != -1){
									map[b][bombx].durability--;
									if(map[b][bombx].durability == 0) map[b][bombx].obj_type = 0;
								}
								damaged_obstacles[damage-1].remaining_durability = map[bomby][b].durability;

                                break;
                            }

                            if(map[b][bombx].obj_type == 2 ||map[b][bombx].obj_type == 4){
									bombers[map[bomby][b].player].alive = -1;
									if(map[bomby][b].obj_type == 2) map[bomby][b].obj_type = 0;
									else map[bomby][b].obj_type = 3;
							}    
						}
                    }
						
					for(int b = bomby;b<=bomby+radius;b++){ //for vertical
						if(b>=0 && b<height && map[b][bombx].obj_type != 0){
                            if(map[b][bombx].obj_type == 1){
								damage++;
								damaged_obstacles = realloc(damaged_obstacles, damage * sizeof(obsd));
								damaged_obstacles[damage-1].position.x = bombx;
								damaged_obstacles[damage-1].position.y = b;
								if(map[b][bombx].durability != -1){
									map[b][bombx].durability--;
									if(map[b][bombx].durability == 0) map[b][bombx].obj_type = 0;
								}
								damaged_obstacles[damage-1].remaining_durability = map[bomby][b].durability;

                                break;
                            }

                            if(map[b][bombx].obj_type == 2 ||map[b][bombx].obj_type == 4){
									bombers[map[bomby][b].player].alive = -1;
									if(map[bomby][b].obj_type == 2) map[bomby][b].obj_type = 0;
									else map[bomby][b].obj_type = 3;
									
						    }                           
					    }
                   }
					
					for(int b=0; b<damage; b++){
						print_output(NULL, NULL, &damaged_obstacles[b], NULL);
					}
				//CLOSE
					close(fds[i].fd);

					waitpid(bomb_pids[j], NULL, 0);
					bomb_pids[j]=-1;
					free(message);
					free(imp_message);
					//free(damaged_obstacles);
			}
		
		}
		
		for(int i=bomb_count, j=0;i<nfds;i++, j++){
			if(game_over) break;
			if(pids[j]==-1)continue;
			if(fds[i].revents & POLLIN){
				
				im* message = (im*) malloc(sizeof(im));

				ssize_t bytes_read = read_data(fds[i].fd, message);
				
				if(bytes_read<0){
					perror("read error");
					exit(1);
				}
				
				imd mdata = message->data;
				//printf(" %d",bytes_read);
				imp* imp_message = (imp*) malloc(sizeof(imp));	
				
				imp_message->m = message;
                imp_message->pid = pids[j];
                print_output(imp_message, NULL, NULL, NULL);


				om* o_message =malloc(sizeof(om));
                                omp* omp_message = malloc(sizeof(omp));


				if(alive_bomber != 1 && bombers[j].alive == -1){ //LOSER
					if(map[bombers[j].position.y][bombers[j].position.x].obj_type == 4) map[bombers[j].position.y][bombers[j].position.x].obj_type = 3;
					else map[bombers[j].position.y][bombers[j].position.x].obj_type = 0;
					alive_bomber--;
							o_message->type = BOMBER_DIE;
							send_message(fds[i].fd, o_message);
							
							omp_message->m = o_message;
							omp_message->pid = pids[j];
							print_output(NULL, omp_message,  NULL, NULL);
							close(fds[i].fd);
							waitpid(pids[j], NULL, 0);
							pids[j]=-1;
							continue;
						}
						else if(alive_bomber == 1){//WINNER
							if(map[bombers[j].position.y][bombers[j].position.x].obj_type == 4) map[bombers[j].position.y][bombers[j].position.x].obj_type = 3;
							else map[bombers[j].position.y][bombers[j].position.x].obj_type = 0;
							alive_bomber--;
							o_message->type = BOMBER_WIN;
							send_message(fds[i].fd, o_message);
							game_over = 1;
							omp_message->m = o_message;
							omp_message->pid = pids[j];
							print_output(NULL, omp_message,  NULL, NULL);
							close(fds[i].fd);
							waitpid(pids[j], NULL, 0);
							pids[j]=-1;
							continue;
						}
				
				
				switch(message->type){	
					
					case BOMBER_START:		
						o_message->type = BOMBER_LOCATION;	
						o_message->data.new_position.x = bombers[j].position.x;
						o_message->data.new_position.y = bombers[j].position.y;
						
						send_message(fds[i].fd, o_message);		
			 			
						omp_message->m = o_message;
						omp_message->pid = pids[j];
						
						print_output(NULL, omp_message, NULL, NULL);		
						break;

					case BOMBER_SEE:
						bomberx = bombers[j].position.x;
						bombery = bombers[j].position.y;
						see=0;
						o_message->type = BOMBER_VISION;
						
						//map[y][x-3]  map[y][x+3] and map[y-3][x] map[y+3][x]
						for(int vis = bomberx-3;vis<bomberx;vis++){ //for horizontal
							if(vis>=0 && vis<width && map[bombery][vis].obj_type != 0){
								see_objects[see].position.x = vis;
                                see_objects[see].position.y = bombery;

								if(map[bombery][vis].obj_type == 1){ //if obstacle
									see_objects[see].type = OBSTACLE;
                                    see++;
                                    break;
                                }

								if(map[bombery][vis].obj_type == 3 ||map[bombery][vis].obj_type == 4){
										see_objects[see].type = BOMB;
										see++;						
								}

								
								if(map[bombery][vis].obj_type == 2 || map[bombery][vis].obj_type == 4){
										see_objects[see].type = BOMBER;
										see++;	
									
								}
							}
						}
						
						for(int vis = bomberx;vis<=bomberx+3;vis++){ //for horizontal
							if(vis>=0 && vis<width && map[bombery][vis].obj_type != 0){
								    see_objects[see].position.x = vis;
                                    see_objects[see].position.y = bombery;

                                if(map[bombery][vis].obj_type== 1){ //if obstacle
									see_objects[see].type = OBSTACLE;
                                    see++;
                                    break;
                                }

                                    if(map[bombery][vis].obj_type == 3 ||map[bombery][vis].obj_type == 4){
										see_objects[see].type = BOMB;
										see++;
									}


                                    if( vis != bomberx && (map[bombery][vis].obj_type == 2 || map[bombery][vis].obj_type == 4)){
										see_objects[see].type = BOMBER;
										see++;	
									} 
                                }
                             }


						for(int vis = bombery-3;vis<bombery;vis++){ //for vertical
							if(vis>=0 && vis<height && map[vis][bomberx].obj_type != 0){
								    see_objects[see].position.x = bomberx;
                                    see_objects[see].position.y = vis;

                                if(map[vis][bomberx].obj_type == 1){
									see_objects[see].type = OBSTACLE;
                                    see++;
                                    break;
                                }

                                if(map[vis][bomberx].obj_type == 3 || map[vis][bomberx].obj_type == 4){
									see_objects[see].type = BOMB;
									see++;
								}

                                if(map[vis][bomberx].obj_type == 2 ||map[vis][bomberx].obj_type == 4){
								    see_objects[see].type = BOMBER;
								    see++;	
									
							    }
                                
						    }
                        }
						

						for(int vis = bombery;vis<=bombery+3;vis++){ //for vertical
							if(vis>=0 && vis<height && map[vis][bomberx].obj_type!= 0){
								    see_objects[see].position.x = bomberx;
                                    see_objects[see].position.y = vis;

                                if(map[vis][bomberx].obj_type == 1){
									see_objects[see].type = OBSTACLE;
                                    see++;
                                    break;
                                }

                                if(vis != bombery && (map[vis][bomberx].obj_type == 3 || map[vis][bomberx].obj_type == 4)){
									see_objects[see].type = BOMB;
									see++;
								}

                                if( vis != bombery && (map[vis][bomberx].obj_type == 2 || map[vis][bomberx].obj_type == 4)){
								    see_objects[see].type = BOMBER;
								    see++;	
							    }    
                            }
                        }
                        
						o_message->data.object_count = see;
						omp_message->m = o_message;
						omp_message->pid = pids[j];

						send_message(fds[i].fd, o_message);
							
						if(see) send_object_data(fds[i].fd, see, see_objects);
						print_output(NULL,omp_message, NULL, see_objects);
						break;

					case BOMBER_MOVE:
						o_message->type = BOMBER_LOCATION;
						//is the move one step away and it is horizontal or vertical?
						if(((bombers[j].position.x+1 == mdata.target_position.x || bombers[j].position.x-1 
						== mdata.target_position.x) && bombers[j].position.y == mdata.target_position.y ) ||
						((bombers[j].position.y+1 == mdata.target_position.y || bombers[j].position.y-1== 
						 mdata.target_position.y) && bombers[j].position.x == mdata.target_position.x)){

							if(mdata.target_position.y<height &&  mdata.target_position.x <width){
								
								if(map[mdata.target_position.y][mdata.target_position.x].obj_type ==0 || map[mdata.target_position.y][mdata.target_position.x].obj_type == 3){
									o_message->data.new_position.x = mdata.target_position.x;
									o_message->data.new_position.y = mdata.target_position.y;

                                    if(map[mdata.target_position.y][mdata.target_position.x].obj_type == 4) //update the old position
                                        map[bombers[j].position.y][bombers[j].position.x].obj_type = 3; //bomb remains
                                    else
                                        map[bombers[j].position.y][bombers[j].position.x].obj_type = 0; //no object

                                    bombers[j].position.x = mdata.target_position.x; 
									bombers[j].position.y = mdata.target_position.y;

									map[bombers[j].position.y][bombers[j].position.x].player = j;

                                    if(map[mdata.target_position.y][mdata.target_position.x].obj_type == 3) //update the new position
                                        map[bombers[j].position.y][bombers[j].position.x].obj_type = 4; //bomb and bomber
                                    else
                                        map[bombers[j].position.y][bombers[j].position.x].obj_type = 2; //only bomber
                                    
									                                                
                            	}	
							}
						}

						else{
							o_message->data.new_position.x = bombers[j].position.x;
							o_message->data.new_position.y = bombers[j].position.y;
						}
						
						send_message(fds[i].fd, o_message);
						omp_message->m = o_message;
						omp_message->pid = bombers[j].pid;
						
						print_output(NULL, omp_message, NULL, NULL);
						break;

					case BOMBER_PLANT:
                        //if(map[bombers[i].position.y][bombers[i].position.x].object)
						o_message->type = BOMBER_PLANT_RESULT;
						if(map[bombers[j].position.y][bombers[j].position.x].obj_type == 2 ){ //there is only bomber
							map[bombers[j].position.y][bombers[j].position.x].obj_type = 4; //put location in bomber 
							bomb_count++;
							remaining_bombs++;
							//printf("bomb count: %d ", bomb_count);
							bombs = realloc(bombs, bomb_count*sizeof(bom));
							bombs[bomb_count-1].position.x = bombers[j].position.x;
							bombs[bomb_count-1].position.y = bombers[j].position.y;
							bombs[bomb_count-1].radius = mdata.bomb_info.radius;
							digit=0;
							for(int b = mdata.bomb_info.interval; b!= 0;b/=10){
								digit++;
							}

							bombs[bomb_count-1].args = malloc(3 *sizeof(char*));
							bombs[bomb_count-1].args[0] = malloc(7 *sizeof(char));
							strcpy(bombs[bomb_count-1].args[0], "./bomb");
							
							bombs[bomb_count-1].args[0][6] = '\0'; //string null

							bombs[bomb_count-1].args[1] = malloc((digit+1)*sizeof(char));
								
							//strcpy(bombs[bomb_count-1].args[1], "5000");
							sprintf(bombs[bomb_count-1].args[1], "%ld", mdata.bomb_info.interval);
							bombs[bomb_count-1].args[1][digit] = '\0';
							bombs[bomb_count-1].args[2] = NULL;

							//printf("%s, %s", bombs[bomb_count-1].args[0], bombs[bomb_count-1].args[1]);

							//create the pipe and execute the bomb
							bomb_fd = realloc(bomb_fd, bomb_count*sizeof(int**));
							bomb_pids = realloc(bomb_pids, bomb_count*sizeof(pid_t));

							bomb_fd[bomb_count-1] = (int*) malloc(2*sizeof(int));
		
							if (PIPE(bomb_fd[bomb_count-1])<0){
								perror("pipe error");		
								exit(1);	
							}	
		
							if((bomb_pids[bomb_count-1] = fork())<0){
								perror("fork error");
								exit(1);
							}
		
							else if(bomb_pids[bomb_count-1]==0){ //child
								close(bomb_fd[bomb_count-1][1]); //child uses fd[0] for r&w

								dup2(bomb_fd[bomb_count-1][0], 0);
								dup2(bomb_fd[bomb_count-1][0], 1);

								//close(bomb_fd[bomb_count-1][0]);
								
								if(execvp(bombs[bomb_count-1].args[0], bombs[bomb_count-1].args)<0){
									perror("exec error");
									exit(1);
								}	//execute the bomber processes
							}

							else{ //parent
								//printf("BOBM: %d", bomb_pids[bomb_count-1]);
								bombs[bomb_count-1].pid = bomb_pids[bomb_count-1];
								close(bomb_fd[bomb_count-1][0]); //parent(controller) uses fd[1] for r&w

							}

							o_message->data.planted = 1; //plant succesful

						} 
						else{
							o_message->data.planted = 0; //plant unsuccesful
						}
						
						send_message(fds[i].fd, o_message);
						omp_message->m = o_message;
						omp_message->pid = bombers[j].pid;
						
						print_output(NULL, omp_message, NULL, NULL);

						break;
					
				
				
				}
				
				free(message);
				free(imp_message);
				free(o_message);
				free(omp_message);

			}
		
		}

		sleep(0.001); //sleep for 1 miliseconds to prevent CPU hogging	
	}

	//free all the mallocs

	for(int i=0;i<bomber_count;i++){
		free(bombers[i].args);
		free(bombers[i].exec_path);
		free(fd[i]);
	}
	free(pids);
	free(fds);
	free(map);
	free(fd);
	free(bombers);
	//free(fds);
	return 0;

}


