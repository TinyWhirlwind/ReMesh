#include "RemeshMesh.h"
#include "SelfIntersection.h"
#include <iostream>
#include "mymesh.h"
#include "wrap/io_trimesh/import.h" 
#include "wrap/io_trimesh/export.h"
#include <vcg/complex/complex.h>

#include "QtCore/qstring.h"
#include "QDir"
#include "QDebug"
#include <windows.h>
#include <commdlg.h>
#include <iostream>
#include <string>
#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>


int main(int argc, char* argv[]) 
{
    QApplication app(argc, argv);

    // 弹出文件选择对话框，允许选择多个文件
    QStringList filePaths = QFileDialog::getOpenFileNames(
        nullptr,
        "选择多个文件",                      // 对话框标题
        QDir::homePath(),                  // 默认路径
        "*.stl;*.obj"  // 文件过滤器
    );

    // 检查用户是否选择了文件
    if (!filePaths.isEmpty())
    {
        CMeshO mesh_;
        CMeshO output_mesh_;
        int mask = 1;
        QString fileList;
        for (const QString& filePath : filePaths) {
            fileList += filePath + "\n";  // 将所有路径拼接为字符串

            QFileInfo fileInfo(filePath);
            fileList += QString("文件名: %1\n绝对路径: %2\n\n")
                .arg(fileInfo.fileName())         // 文件名
                .arg(fileInfo.absoluteFilePath()); // 绝对路径
            std::string file_name = fileInfo.fileName().toUtf8().constData();
            std::string input_file_name = fileInfo.absoluteFilePath().toUtf8().constData();
            qDebug() << fileInfo.absoluteFilePath();
            std::cout << input_file_name.c_str() << std::endl;
            vcg::tri::io::ImporterSTL<CMeshO>::OpenBinary(mesh_, input_file_name.c_str(), mask);
            RemeshMesh rm(mesh_);
            rm.apply();
            rm.getMesh(output_mesh_);
            vcg::tri::io::ExporterSTL<CMeshO>::Save(output_mesh_, input_file_name.data());
        }
    }
#if 0 
    CMeshO mesh_;
    CMeshO output_mesh_;
    int mask = 1;
    QString strWorkSpace = "D:/data/Remeshing";
    QDir dir(strWorkSpace);
    ///获取file文件夹中所有后缀名为“.text”的文件，并转为QFileInfo保存到fileList中
    QFileInfoList fileList = dir.entryInfoList(QStringList() << "*jig.stl");
    for (auto& fileInfo : fileList)
    {
        std::string file_name = fileInfo.fileName().toUtf8().constData();
        std::string input_file_name = fileInfo.absoluteFilePath().toUtf8().constData();
        qDebug() << fileInfo.absoluteFilePath();
        std::cout << input_file_name.c_str() << std::endl;
        vcg::tri::io::ImporterSTL<CMeshO>::OpenBinary(mesh_, input_file_name.c_str(), mask);
        RemeshMesh rm(mesh_);
        rm.apply();
        rm.getMesh(output_mesh_);
        vcg::tri::io::ExporterSTL<CMeshO>::Save(output_mesh_, input_file_name.data());
        //SelfIntersection::mergeNearPoint(input_file_name, input_file_name);
    }
#endif
}
