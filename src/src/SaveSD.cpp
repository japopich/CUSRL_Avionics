#include "yonics.hpp"

#define charLen 14

SaveSD::SaveSD() {
    if (sd.begin()) {
        running = true;
    }
}
bool SaveSD::addFlashOp(FlashOp* flash) {
    if (this->flash==NULL) {
        this->flash = flash;
        return true;
    }
    else {return false;}
}

void SaveSD::printRCS(double current, double omega, double ctrl_torque, bool isOpen){
    Serial.println("Saving: timestamp,current,omega,ctrl_torque,isOpen");
    of.printf("%u,%.4f,%.4f,%.4f,%u\n", millis(), current, omega, ctrl_torque, isOpen);

    
}


bool SaveSD::saveNowRCS (double current, double omega, double ctrl_torque, bool isOpen) {
    if (!running) {return false;}

//    uint16_t counter = 1;
//    int charLen = 14;
    char filename [charLen];
    sprintf(filename,"datalog_rcs.csv");
    of = sd.open(filename,FILE_WRITE);

    Serial.println("File opened!");

    // printEVENTS();
    printRCS(current, omega, ctrl_torque, isOpen);
    of.close();

    return true;
}

bool SaveSD::savenow () {
    if (!running) {return false;}

    if (!flash->beginRead()) {return false;}

    uint16_t counter = 1;
//    int charLen = 14;
    char filename [charLen];

    while(true) {
        sprintf(filename,"datalog%03d.csv",counter);

        if(sd.exists(filename)) {
            Serial.println("Iterating file counter.");
            counter+=1;
            if(counter>999){return false;}
        }
        else
        {
            break;
        }
        
    }
    of = sd.open(filename,FILE_WRITE);

    Serial.println("File opened!");

    // printEVENTS();
    printIMU();
    printBAROM();
    printACCEL();
    printGPS();

    of.close();

    return true;
}



void SaveSD::printEVENTS() {
    of.println("# Events:");
    of.println("t, ID");

    int counter = 0;

    uint32_t t;
    char c;

    while(flash->getEvent(counter++,&t,&c)) {
        of.printf("%d,%c\n",t,c);
    }
}

void SaveSD::printIMU() {
    of.println("# IMU:");
    of.println("orient_euler_x,orient_euler_y,orient_euler_z,gyro_fused_x,gyro_fused_y,gyro_fused_z,accel_fused_x,accel_fused_y,accel_fused_z,accel_raw_x,accel_raw_y,accel_raw_z,gyro_raw_x,gyro_raw_y,gyro_raw_z,magnetometer_x,magnetometer_y,magnetometer_z,orient_quat_w,orient_quat_x,orient_quat_y,orient_quat_z,");

    // Serial.print("Saving IMU data...");
    // Save IMU data

    uint32_t counter = 0;

    while(flash->getSample(imuID,counter++,&tempIMU)) {
        
        of.printf("%.4f,%.4f,%.4f",tempIMU.orient_euler[0],tempIMU.orient_euler[1],tempIMU.orient_euler[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.gyro_fused[0],tempIMU.gyro_fused[1],tempIMU.gyro_fused[2]);
        of.print(",");
        
        of.printf("%.4f,%.4f,%.4f",tempIMU.accel_fused[0],tempIMU.accel_fused[1],tempIMU.accel_fused[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.accel_raw[0],tempIMU.accel_raw[1],tempIMU.accel_raw[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.gyro_raw[0],tempIMU.gyro_raw[1],tempIMU.gyro_raw[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f",tempIMU.magnetometer[0],tempIMU.magnetometer[1],tempIMU.magnetometer[2]);
        of.print(",");

        of.printf("%.4f,%.4f,%.4f,%.4f",tempIMU.orient_quat[0],tempIMU.orient_quat[1],tempIMU.orient_quat[2],tempIMU.orient_quat[3]);
        of.print(",");

        of.printf("%u",tempIMU.t);
        of.println("");

        Serial.println("IMU sample saved.");
    }
}

void SaveSD::printBAROM() {
    of.println("# Barometer:");
    of.println("Altitude,Pressure,Temperature");

    uint32_t counter=0;

    // Save barometer data
    while(flash->getSample(1,counter++,&tempBAROM)) {
        of.printf("%.4f,%.4f,%.4f\n",tempBAROM.altitude,tempBAROM.pressure,tempBAROM.temperature);
    }
}

void SaveSD::printACCEL() {
    of.println("# Accelerometer:");
    of.println("x,y,z");

    uint32_t counter = 0;

    while(flash->getSample(2,counter++,&tempACCEL)) {
        of.printf("%.4f,%.4f,%.4f\n",tempACCEL.x,tempACCEL.y,tempACCEL.z);
    }
}

void SaveSD::printGPS() {
    of.println("# GPS:");
    of.println("lat,lom,altitude,speed,angle,sat_num");

    uint32_t counter = 0;

    while(flash->getSample(3,counter++,&tempGPS)) {
        of.printf("%.10f,%.10f,%.4f,%.4f,%.10f,%f\n",tempGPS.lat,tempGPS.lon,tempGPS.altitude,tempGPS.speed,tempGPS.angle,tempGPS.sat_num);
    }

}

bool SaveSD::openFile() {
    uint16_t counter = 1;
    //int charLen = 14;
    char filename [charLen];

    while(true) {
        sprintf(filename,"datalog%03d.csv",counter);

        if(sd.exists(filename)) {
            counter+=1;
            if(counter>999){return false;}
        }
        else {
            if (sd.open(filename, FILE_WRITE)) {return true;}
            else {return false;}
        }
    }
}
