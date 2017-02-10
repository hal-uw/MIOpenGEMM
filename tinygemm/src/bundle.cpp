#include <tinygemm/bundle.hpp>
#include <tinygemm/tinygemmerror.hpp>
#include <tinygemm/derivedparams.hpp>
#include <tinygemm/betagenerator.hpp>
#include <tinygemm/alphagenerator.hpp>
#include <tinygemm/copygenerator.hpp>
#include <tinygemm/stringutilbase.hpp>

#include <string>
#include <iostream>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <cmath>
#include <tuple>
#include <fstream>
#include <chrono>
#include <ctime>
#include <algorithm>

namespace tinygemm{
namespace kerngen{




class BundleGenerator{




private:
  const hyperparams::HyperParams & hp;
  const tinygemm::TinyGemmGeometry & gg;
  /* to be set in constructor based on parameters provided */
  const derivedparams::DerivedParams dp;
  
  //TODO: should be static:
  std::map <std::string, std::vector<std::string> > depmap;

public: 
  BundleGenerator(const hyperparams::HyperParams & hp_, const tinygemm::TinyGemmGeometry & gg_): hp(hp_), gg(gg_), dp(hp, gg) {
    
    //TODO: should be static:    
    depmap["copy_a"] = {};
    depmap["copy_b"] = {};
    depmap["betac"] = {};
    depmap["alphaab"] = {"betac", "copy_a", "copy_b"};
    depmap["betac_alphaab"] = {"betac", "copy_a", "copy_b"};

  }

  Bundle generate(){
    
    
    std::vector<KernelString> v_tgks;  
    std::vector<std::vector<unsigned> > v_wait_indices;
    
    if (hp.a_copy_workspace == 1){
      
      
      //KernelString get_copya_kernelstring(const tinygemm::TinyGemmGeometry & gg, const tinygemm::derivedparams::DerivedParams & dp){
 //ForallGenerator fg(gg, dp, "copya");
 //return fg.get_forall_kernelstring();
//}


      v_tgks.emplace_back( betagen::get_copya_kernelstring(gg, dp) );
    }
    
    if (hp.b_copy_workspace == 1){
      //v_tgks.emplace_back( copygen::get_copy_b_kernelstring(hp, gg, dp) ); //deduce from hp whether a is copied or not. 
    }
    

    
    if (dp.does_beta_c_inc == 0){
      /* (OLD) currently the betac kernel does not rely on any hyper parameters, this may change. */
      v_tgks.emplace_back( betagen::get_beta_kernelstring(gg, dp) );
    }
    
    v_tgks.emplace_back( alphagen::get_alpha_kernelstring(hp, gg, dp) );
    
    
    

    /* indent the kernel strings, in case someone wants to print them. Performance addicts would not do this */
    for (auto & x : v_tgks){
      stringutil::indentify(x.kernstr);
    }


    std::vector<std::string> types;
    for (unsigned i = 0; i < v_tgks.size(); ++i){
      types.push_back(v_tgks[i].type);
    }

    for (unsigned i = 0; i < v_tgks.size(); ++i){
      v_wait_indices.push_back({});
      for (unsigned j = 0; j < v_tgks.size(); ++j){
        if (std::find(depmap.at(types[i]).begin(), depmap.at(types[i]).end(), types[j]) != depmap.at(types[i]).end()) {
          v_wait_indices.back().push_back(j);
        }
      }
    }

    
    std::cout << "-----------------------------------------\nnetwork : \n";
    for (unsigned i = 0; i < v_tgks.size(); ++i){
      std::cout << "kernel " << i << " ( " << types[i] << " )  ----- waits for -----> " << std::flush;
      for (unsigned j = 0; j < v_wait_indices[i].size(); ++j){
        std::cout << v_wait_indices[i][j] << " ( " << types[v_wait_indices[i][j]] << " )   " << std::flush;
      }
      std::cout << std::endl;
    }
    std::cout << "-----------------------------------------\n" << std::endl;
    


    
    return { std::move(v_tgks), std::move(v_wait_indices), std::move(dp) };
  }
};




Bundle get_bundle(const hyperparams::HyperParams & hp,  const tinygemm::TinyGemmGeometry & gg){  
  BundleGenerator ksbg(hp, gg);
  return ksbg.generate();
}



}
}
