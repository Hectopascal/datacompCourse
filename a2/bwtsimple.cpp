

//dumb method
std::string decodeInput1(std::string input){
    int N = input.length();
    std::vector<std::vector<char>> matrix(N,std::vector<char>());    
    for (int i = 0; i < N; ++i){
        for(int j = 0; j<N; ++j){
            matrix.at(j).insert(matrix.at(j).begin() ,input.at(j));
        } 
        std::sort(matrix.begin(),matrix.end(),sortVectorLex);

    }
      
    for(int k =0;k < N; ++k){
        if (matrix.at(k).back() == '\n'){
            std::string s{matrix.at(k).begin(),matrix.at(k).end()};
            return s;
        }
    }
    return "";
}

