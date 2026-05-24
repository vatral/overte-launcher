#include <QTest>
#include <QSignalSpy>


#include "../s3bucketlister.h"
#include "../s3bucketfilter.h"




const QUrl BUCKET_URL{"https://public.overte.org"};



// add necessary includes here

class FilterTest : public QObject
{
    Q_OBJECT

public:
    FilterTest();
    ~FilterTest() override;

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();
    void test_filterLinux();
    void test_filterWindows();
    void test_filterMac();
private:
    QList<S3BucketLister::FileData> _foundFiles;
};

FilterTest::FilterTest() {}

FilterTest::~FilterTest() = default;

void FilterTest::initTestCase()
{
    // code to be executed before the first test function

    S3BucketLister lst;



    QSignalSpy spyFiles(&lst, &S3BucketLister::filesFound);
    QSignalSpy spyErrors(&lst, &S3BucketLister::errorOccurred);
    QSignalSpy spyDone(&lst, &S3BucketLister::listingDone);

    connect(&lst, &S3BucketLister::filesFound, this, [this](const QList<S3BucketLister::FileData> &files) {
        qInfo() << "Received files:" << files.size();
        _foundFiles.append(files);
    });

    lst.listBucket(BUCKET_URL);

    QVERIFY(spyFiles.isValid());
    QVERIFY(spyErrors.isValid());
    QVERIFY(spyDone.isValid());

    spyDone.wait(5000);

    QCOMPARE(spyDone.count(), 1);
    QCOMPARE(spyErrors.count(), 0);
    QVERIFY(spyFiles.count() > 0 );

    qInfo() << "Starting test with" << _foundFiles.size() << "files";



}

void FilterTest::init()
{
    // code to be executed before each test function
}

void FilterTest::cleanupTestCase()
{
    // code to be executed after the last test function
}

void FilterTest::cleanup()
{
    // code to be executed after each test function
}

void FilterTest::test_filterLinux() {

    S3BucketFilter filter;
    filter.setOS(S3BucketFilter::OperatingSystem::Linux);
    auto results = filter.filter(_foundFiles);

    QVERIFY(results.count() > 0);
}


void FilterTest::test_filterWindows() {

    S3BucketFilter filter;
    filter.setOS(S3BucketFilter::OperatingSystem::Windows);
    auto results = filter.filter(_foundFiles);

    QVERIFY(results.count() > 0);
}

void FilterTest::test_filterMac() {

    S3BucketFilter filter;
    filter.setOS(S3BucketFilter::OperatingSystem::MacOS);
    auto results = filter.filter(_foundFiles);

    QVERIFY(results.count() > 0);
}


QTEST_MAIN(FilterTest)

#include "tst_filtertest.moc"
