// I wrote this program to help myself understand how to control and understand Kokkos Team Policies. 
// You can only call these member variables inside of parallel regions, so its a bit more challenging 
// if you are coming from openmp where you can say something like (e.g., omp_get_num_processors()) 
// anywhere in the program. BTW, if you compiled for OpenMP, you can omp_get_num_processors() normally 
// nut the functions implemented below require the parallel regon to query. Hopefully it will help you
// like it helped me! 

#include <Kokkos_Core.hpp>
#include <iostream> 

int main()
{

      Kokkos::initialize(); 
      { /*start kokkos scope*/ 
      typedef Kokkos::TeamPolicy<>::member_type member_type; 
      Kokkos::TeamPolicy<> policy(2, Kokkos::AUTO() ); //2 teams, let kokkos decide num threads per team 
      
      /*launch parallel region using our policy we created on line 17 
        query member vars with team_member*/ 
      Kokkos::parallel_for(policy, KOKKOS_LAMBDA (member_type team_member)
      { 
          int threadID = team_member.team_rank(); /*what thread am i on this team*/
          int teamID = team_member.league_rank(); /*what team am i on */ 
          int num_teams = team_member.league_size(); /*how many teams*/ 
          int threads_per_team = team_member.team_size(); 
          std::cout << "\nTeamID: " << teamID; 
          std::cout << "\nThreadID: " << threadID; 
          std::cout << "\nNumber of Teams: " << num_teams;
          std::cout << "\nThreads/team: " << threads_per_team; 
     }); 
     
      Kokkos::finalize();
      } /* close kokkos scope here*/      
}
