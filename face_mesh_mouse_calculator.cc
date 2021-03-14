#include <fstream>
#include <cstdlib>
#include <cmath>

#include "gnuplot.h"
#include "mediapipe/framework/api2/node.h"
#include "mediapipe/framework/calculator_framework.h"
#include "mediapipe/framework/port/ret_check.h"
#include "mediapipe/framework/formats/landmark.pb.h"
#include "mediapipe/framework/formats/rect.pb.h"
#include "mediapipe/framework/formats/detection.pb.h"

namespace mediapipe {

namespace api2 {
class FaceInfoWritePlotCalculator : public Node {
 public:
 
  // input value
  static constexpr Input<std::vector<mediapipe::NormalizedLandmarkList, std::allocator<mediapipe::NormalizedLandmarkList>>> kInLandmarks1{"LANDMARKS"}; //kInLandmarks3
  static constexpr Input<std::vector<mediapipe::NormalizedRect, std::allocator<mediapipe::NormalizedRect>>> kInLandmarks2{"NORM_RECTS"};
  // static constexpr Input<std::vector<mediapipe::Detection, std::allocator<mediapipe::Detection> >> kInLandmarks3{"DETECTIONS"};
  static constexpr Output<int>::Optional kOutUnused{"INT"};
  
  MEDIAPIPE_NODE_INTERFACE(FaceInfoWritePlotCalculator, kInLandmarks1, kInLandmarks2, kOutUnused);
  
  static mediapipe::Status UpdateContract(CalculatorContract* cc) {

    return mediapipe::OkStatus();
  }

  mediapipe::Status Open(CalculatorContext* cc) final {
    // out_path = getenv("HOME");
    // out_path += "/landmarks.csv";
    // file.open(out_path);

    // out_path = getenv("HOME");
    // out_path += "/landmarks2.csv";
    // file2.open(out_path);


    // RET_CHECK(file);
    // file <<"x, y" << std::endl;
    
    return mediapipe::OkStatus();
  }
  
  mediapipe::Status Process(CalculatorContext* cc) final {

    const std::vector<mediapipe::NormalizedLandmarkList, std::allocator<mediapipe::NormalizedLandmarkList>>& LANDMARKS = *kInLandmarks1(cc);
    const std::vector<mediapipe::NormalizedRect, std::allocator<mediapipe::NormalizedRect>>& NORM_RECTS = *kInLandmarks2(cc);
    // const std::vector<mediapipe::Detection, std::allocator<mediapipe::Detection>> DETECTIONS = *kInLandmarks3(cc);
    // std::cout<<"==========Info=========\n";
    // std::cout<<"LAND MARKS"<<std::endl;
    // std::cout<<LANDMARKS[0].landmark_size()<<std::endl;
    // // std::cout<<LANDMARKS[0].label().data()<<std::endl;
    // std::cout<<LANDMARKS[1].landmark_size()<<std::endl;
    
    //윗 입술 : 0
    //아랫 입술 : 17
    //왼쪽 입술 : 76 
    //오른쪽 입술 : 306

    //입 관련 
    const mediapipe::NormalizedLandmark& upper_lips=LANDMARKS[0].landmark(0);
    const mediapipe::NormalizedLandmark& lower_lips=LANDMARKS[0].landmark(17);
    const mediapipe::NormalizedLandmark& left_lips=LANDMARKS[0].landmark(76);
    const mediapipe::NormalizedLandmark& right_lips=LANDMARKS[0].landmark(306);

    // std::cout<<"NORM_RECTS"<<std::endl;
    // std::cout<<"rect_id: " << NORM_RECTS[0].rect_id()<<std::endl;
    // std::cout<<"height: " << NORM_RECTS[0].height()<<std::endl;
    // std::cout<<"width : " <<NORM_RECTS[0].width()<<std::endl;
    // std::cout<<"x_center : " << NORM_RECTS[0].x_center()<<std::endl;
    // std::cout<<"y_center : " << NORM_RECTS[0].y_center()<<std::endl;
    // std::cout<<std::endl;
    // std::cout<<"upper lips : "<<upper_lips.x()<<","<<upper_lips.y()<<std::endl;
    // std::cout<<std::endl;  

    // std::cout<<left_lips.z()<<","<<right_lips.z()<<std::endl;
    float norm_rects_start_x=NORM_RECTS[0].x_center()-(NORM_RECTS[0].width()/2);
    float norm_rects_start_y=NORM_RECTS[0].y_center()-(NORM_RECTS[0].height()/2);
    float norm_rects_renormalize_factor_x=1/(NORM_RECTS[0].width());
    float norm_rects_renormalize_factor_y=1/(NORM_RECTS[0].width());

    float upper_lips_renormalized_x=norm_rects_renormalize_factor_x*(upper_lips.x()-norm_rects_start_x);
    float upper_lips_renormalized_y=norm_rects_renormalize_factor_y*(upper_lips.y()-norm_rects_start_y);
    float lower_lips_renormalized_x=norm_rects_renormalize_factor_x*(lower_lips.x()-norm_rects_start_x);
    float lower_lips_renormalized_y=norm_rects_renormalize_factor_y*(lower_lips.y()-norm_rects_start_y);

    float upper_lower_distance=std::sqrt(std::pow((upper_lips_renormalized_x-lower_lips_renormalized_x),2)+std::pow((upper_lips_renormalized_y-lower_lips_renormalized_y),2));
    // float upper_lower_distance=std::sqrt(std::pow((upper_lips.x()-lower_lips.x()),2)+std::pow((upper_lips.y()-lower_lips.y()),2));
    // float left_right_distance=std::sqrt(std::pow((left_lips.x()-right_lips.x()),2)+std::pow((left_lips.y()-right_lips.y()),2));
    
    // std::cout<<upper_lips_renormalized_x<<","<<lower_lips_renormalized_x<<std::endl;
    // std::cout<<upper_lower_distance<<std::endl;
    // std::cout<<"================================="<<std::endl;
    

    if(upper_lower_distance>0.12){
      std::cout<<"talking"<<","<<upper_lower_distance<<std::endl;
    }
    else{
      std::cout<<"Normal"<<","<<upper_lower_distance<<std::endl;
    }
    
    
    // if (upper_lower_distance>left_right_distance){
    //   // std::cout<<"upper"<<std::endl;
    // }
    // else{
    //   // std::cout<<"left"<<std::endl;
    // }

    //얼굴 관련 
    const mediapipe::NormalizedLandmark& left_face=LANDMARKS[0].landmark(123);
    const mediapipe::NormalizedLandmark& right_face=LANDMARKS[0].landmark(352);
    float left_face_z=left_face.z();
    float right_face_z=right_face.z();
    
    float z_variance=left_face_z-right_face_z;
    float z_variance_abs=std::abs(left_face_z-right_face_z);
    float threshold=0.08;
    if(z_variance_abs>threshold){
      if(z_variance>0){
        // std::cout<<"Anomaly"<<","<<"right face"<<std::endl;
      }
      else{
        // std::cout<<"Anomaly"<<","<<"left face"<<std::endl;
      }
      
    }
    else{
      // std::cout<<"Normal"<<std::endl;
    }

    // std::cout<<"NORM_RECTS"<<std::endl;
    // std::cout<<"rect_id: " << NORM_RECTS[0].rect_id()<<std::endl;
    // std::cout<<"height: " << NORM_RECTS[0].height()<<std::endl;
    // std::cout<<"width : " <<NORM_RECTS[0].width()<<std::endl;
    // std::cout<<"x_center : " << NORM_RECTS[0].x_center()<<std::endl;
    // std::cout<<"y_center : " << NORM_RECTS[0].y_center()<<std::endl;
    // std::cout<<std::endl;
    // std::cout<<"upper lips : "<<upper_lips.x()<<","<<upper_lips.y()<<std::endl;
    // std::cout<<std::endl;  












    //legacy

    // std::cout<<z_variance_abs<<std::endl;
    
  
   
    // if (LANDMARKS[0].landmark_size() == 468){

    // for(int i = 0; i < 1; ++i){
    //     const mediapipe::NormalizedLandmark& lm = LANDMARKS[0].landmark(i);
    //     std::cout<<lm.x()<<","<<lm.y()<<","<<lm.z()<<","<<std::endl;
        
    // }
    
    // else{
    //   for(int i = 0; i < 468; ++i){
    //     // file<<0.0<<","<< 0.0<<std::endl;
    //   }


    // }
    // int landmark_size = LANDMARKS[1].landmark_size();
    // if(landmark_size == 7){
    //   std::cout << landmark_size<<std::endl;
    //   for(int i = 0; i < 7; ++i){
    //     file<<LANDMARKS[1].landmark(i).x()<<","<< LANDMARKS[1].landmark(i).y()<<std::endl;
    //   }
    // }

    // std::cout<<"NORM_RECTS"<<std::endl;
    // std::cout<<"rect_id: " << NORM_RECTS[0].rect_id()<<std::endl;
    // std::cout<<"height: " << NORM_RECTS[0].height()<<std::endl;
    // std::cout<<"width : " <<NORM_RECTS[0].width()<<std::endl;
    // std::cout<<"x_center : " << NORM_RECTS[0].x_center()<<std::endl;
    // std::cout<<"y_center : " << NORM_RECTS[0].y_center()<<std::endl;
    // std::cout<<std::endl;    
    // std::cout<<NORM_RECTS[1].rect_id();
    // std::cout<<NORM_RECTS[1].height();
    // std::cout<<NORM_RECTS[1].width();
    // std::cout<<NORM_RECTS[1].x_center();
    // std::cout<<NORM_RECTS[1].y_center();
    // std::cout<<std::endl;    
    // std::cout<<"DETECTIONS"<<std::endl;
    // std::cout<<"label_size : " << DETECTIONS[0].label_size()<<std::endl;
    // std::cout<<"label :" <<DETECTIONS[0].label().data()<<std::endl;
    // // std::cout<<DETECTIONS[0].label_id()<<std::endl;
    // std::cout<<"label_id_size :" <<DETECTIONS[0].label_id_size()<<std::endl;
    // // std::cout<<DETECTIONS[0].location_data()<<std::endl;
    // std::cout<<"display_name_size :" <<DETECTIONS[0].display_name_size()<<std::endl;
    // std::cout<<"display_name :" <<DETECTIONS[0].display_name().data()<<std::endl;
    // std::cout<<"has_detection_id :" <<DETECTIONS[0].has_detection_id()<<std::endl;
    




    // for(int i = 0; i < LANDMARKS[0].landmark_size(); ++i){
    //   const mediapipe::NormalizedLandmark& lm = LANDMARKS[0].landmark(i);
    //   std::cout<<lm.x()<<std::endl;
    //   std::cout<<lm.y()<<std::endl;
    //   std::cout<<lm.visibility()<<std::endl;
    //   std::cout<<"============================================================================"<<std::endl;
    // }
    // std::cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<std::endl;
    
    // for(int i = 0; i < LANDMARKS[1].landmark_size(); ++i){
    //   const mediapipe::NormalizedLandmark& lm = LANDMARKS[0].landmark(i);
    //   std::cout<<lm.x()<<std::endl;
    //   std::cout<<lm.y()<<std::endl;
    //   std::cout<<lm.visibility()<<std::endl;
    //   std::cout<<"============================================================================"<<std::endl;
    // }

    // for(int i = 0; i< )

    // float change_rate = 0;
    // float center_coord_x= 0.0;
    // float center_coord_y= 0.0;
    // float difference = 0.0;

    // const mediapipe::NormalizedLandmark& center = LANDMARKS.landmark(0);

    // for (int i = 0; i < LANDMARKS[0].landmark_size(); ++i) {
    //   const mediapipe::NormalizedLandmark& landmark = LANDMARKS[0].landmark(i);
    //   // center_coord_x += landmark.x();
    //   // center_coord_y += landmark.y();
    //   //file << i << ':' << landmark.x() << ',' << landmark.y() << ',';
    //   //file << std::endl;
    // }
    
    // center_coord_x /= 71;
    // center_coord_y /= 71;      
    // change_rate = (pow(center.x()-center_coord_x,2) + pow(center.y()-center_coord_y,2))*1000;
     
    // file << change_rate;        
    // file << std::endl;
    
    // std::string home = getenv("HOME");
    // GnuplotPipe gp;
    // gp.sendLine("set datafile separator ','");
    // gp.sendLine("set terminal png");
    // gp.sendLine("set output '"+home+"/plot.png'");
    // gp.sendLine("set xlabel 'frames'");
    // gp.sendLine("set yrange [0:0.3]");
    // gp.sendLine("set ytics 0.01");
    // gp.sendLine("plot '"+out_path+"' using 'changed_rate' with linespoint ls 1 pt 5");

    return mediapipe::OkStatus();
  }
  
 private:
  std::ofstream file;
  std::string file_path;
  std::ofstream file2;
  std::string file2_path;
        
  std::string out_path;
};

MEDIAPIPE_REGISTER_NODE(FaceInfoWritePlotCalculator);
}

}
