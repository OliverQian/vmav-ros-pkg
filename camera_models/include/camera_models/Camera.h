#ifndef CAMERA_H
#define CAMERA_H

#include <boost/shared_ptr.hpp>
#include <Eigen/Eigen>
#include <opencv2/core/core.hpp>
#include <px_comm/CameraInfo.h>
#include <vector>

namespace px
{

class Camera
{
public:
    enum ModelType
    {
        KANNALA_BRANDT,
        MEI,
        PINHOLE
    };

    class Parameters
    {
    public:
        Parameters(ModelType modelType);

        Parameters(ModelType modelType,
                   const std::string& cameraName,
                   const std::string& cameraType,
                   int w, int h);

        ModelType& modelType(void);
        std::string& cameraName(void);
        std::string& cameraType(void);
        int& imageWidth(void);
        int& imageHeight(void);

        ModelType modelType(void) const;
        const std::string& cameraName(void) const;
        const std::string& cameraType(void) const;
        int imageWidth(void) const;
        int imageHeight(void) const;

        int nIntrinsics(void) const;

        virtual bool readFromYamlFile(const std::string& filename) = 0;
        virtual void writeToYamlFile(const std::string& filename) const = 0;

    protected:
        ModelType m_modelType;
        int m_nIntrinsics;
        std::string m_cameraName;
        std::string m_cameraType;
        int m_imageWidth;
        int m_imageHeight;
    };

    Camera();

    virtual int& cameraId(void);
    virtual int cameraId(void) const;

    virtual ModelType modelType(void) const = 0;
    virtual const std::string& cameraName(void) const = 0;

    virtual std::string& cameraType(void) = 0;
    virtual const std::string& cameraType(void) const = 0;

    virtual int imageWidth(void) const = 0;
    virtual int imageHeight(void) const = 0;

    virtual cv::Mat& mask(void);
    virtual const cv::Mat& mask(void) const;

    virtual void setZeroDistortion(void) = 0;

    virtual void estimateIntrinsics(const cv::Size& boardSize,
                                    const std::vector< std::vector<cv::Point3f> >& objectPoints,
                                    const std::vector< std::vector<cv::Point2f> >& imagePoints) = 0;
    virtual void estimateExtrinsics(const std::vector<cv::Point3f>& objectPoints,
                                    const std::vector<cv::Point2f>& imagePoints,
                                    cv::Mat& rvec, cv::Mat& tvec) const;

    // Lift points from the image plane to the sphere
    virtual void liftSphere(const Eigen::Vector2d& p, Eigen::Vector3d& P) const = 0;
    //%output P

    // Lift points from the image plane to the projective space
    virtual void liftProjective(const Eigen::Vector2d& p, Eigen::Vector3d& P) const = 0;
    //%output P

    // Projects 3D points to the image plane (Pi function)
    virtual void spaceToPlane(const Eigen::Vector3d& P, Eigen::Vector2d& p) const = 0;
    //%output p

    // Projects 3D points to the image plane (Pi function)
    // and calculates jacobian
    virtual void spaceToPlane(const Eigen::Vector3d& P, Eigen::Vector2d& p,
                              Eigen::Matrix<double,2,3>& J) const = 0;
    //%output p
    //%output J

    virtual void undistToPlane(const Eigen::Vector2d& p_u, Eigen::Vector2d& p) const = 0;
    //%output p

    virtual void initUndistortMap(cv::Mat& map1, cv::Mat& map2) const = 0;
    virtual cv::Mat initUndistortRectifyMap(cv::Mat& map1, cv::Mat& map2,
                                            float fx = -1.0f, float fy = -1.0f,
                                            cv::Size imageSize = cv::Size(0, 0),
                                            float cx = -1.0f, float cy = -1.0f,
                                            cv::Mat rmat = cv::Mat::eye(3, 3, CV_32F)) const = 0;

    virtual void readParameters(const std::vector<double>& parameters) = 0;
    virtual void writeParameters(std::vector<double>& parameters) const = 0;

    virtual void writeParametersToYamlFile(const std::string& filename) const = 0;

    virtual void readParameters(const px_comm::CameraInfoConstPtr& cameraInfo) = 0;
    virtual void writeParameters(px_comm::CameraInfoPtr& cameraInfo) const = 0;

    virtual std::string parametersToString(void) const = 0;

    /**
     * \brief Calculates the reprojection distance between points
     *
     * \param P1 first 3D point coordinates
     * \param P2 second 3D point coordinates
     * \return euclidean distance in the plane
     */
    double reprojectionDist(const Eigen::Vector3d& P1, const Eigen::Vector3d& P2) const;

    double reprojectionError(const std::vector< std::vector<cv::Point3f> >& objectPoints,
                             const std::vector< std::vector<cv::Point2f> >& imagePoints,
                             const std::vector<cv::Mat>& rvecs,
                             const std::vector<cv::Mat>& tvecs,
                             cv::OutputArray perViewErrors = cv::noArray()) const;

    double reprojectionError(const Eigen::Vector3d& P,
                             const Eigen::Quaterniond& camera_q,
                             const Eigen::Vector3d& camera_t,
                             const Eigen::Vector2d& observed_p) const;

    void projectPoints(const std::vector<cv::Point3f>& objectPoints,
                       const cv::Mat& rvec,
                       const cv::Mat& tvec,
                       std::vector<cv::Point2f>& imagePoints) const;
protected:
    template<typename T>
    T square(T x) const;

    int m_cameraId;
    cv::Mat m_mask;
};

typedef boost::shared_ptr<Camera> CameraPtr;
typedef boost::shared_ptr<const Camera> CameraConstPtr;

template<typename T>
T
Camera::square(T x) const
{
    return x * x;
}

}

#endif
