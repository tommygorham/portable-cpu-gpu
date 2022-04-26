#include <Kokkos_Core.hpp>
#include <iostream> 

int main()
{

      Kokkos::initialize(); 
      
      typedef Kokkos::TeamPolicy<>::member_type member_type; 
      Kokkos::TeamPolicy<> policy(2, Kokkos::AUTO() ); //2 teams, let kokkos decide num threads per team 

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
      }
      
      }