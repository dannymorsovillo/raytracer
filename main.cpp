#include <iostream>

int main(){

    // image
    int image_width = 256;
    int image_height = 256;

    //Render
    std::cout<<"P3\n"<<image_width<<' '<<image_height<<"\n225\n";

    for (int i = 0; i < image_height; i++){
        for (int j = 0; j < image_width; j++){
            auto x = double(i) / (image_height-1);
            auto y = double(j) / (image_width - 1);
            auto z = 0.0;

            int ix = int(255.999 * x);
            int iy = int(255.999 * y);
            int iz = int(255.999 * z);

            std::cout<<ix<<' '<<iy<<' '<<iz<<'\n';
            
        }
    }
    
    return 0;
}