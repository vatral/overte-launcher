#include <QTest>
#include <QSignalSpy>


#include "../s3bucketlister.h"
#include "../s3bucketfilter.h"




const QUrl BUCKET_URL{"https://public.overte.org"};



// add necessary includes here

class ManifestDownloaderTest : public QObject
{
    Q_OBJECT

public:
    ManifestDownloaderTest();
    ~ManifestDownloaderTest() override;

private slots:
    void initTestCase();
    void init();
    void cleanupTestCase();
    void cleanup();
    void test_listBucket();
};

ManifestDownloaderTest::ManifestDownloaderTest() {}

ManifestDownloaderTest::~ManifestDownloaderTest() = default;

void ManifestDownloaderTest::initTestCase()
{
    // code to be executed before the first test function
}

void ManifestDownloaderTest::init()
{
    // code to be executed before each test function
}

void ManifestDownloaderTest::cleanupTestCase()
{
    // code to be executed after the last test function
}

void ManifestDownloaderTest::cleanup()
{
    // code to be executed after each test function
}

void ManifestDownloaderTest::test_listBucket() {
    S3BucketLister lst;
    QSignalSpy spyFiles(&lst, &S3BucketLister::filesFound);
    QSignalSpy spyErrors(&lst, &S3BucketLister::errorOccurred);
    QSignalSpy spyDone(&lst, &S3BucketLister::listingDone);

    lst.listBucket(BUCKET_URL);

    QVERIFY(spyFiles.isValid());
    QVERIFY(spyErrors.isValid());
    QVERIFY(spyDone.isValid());

    spyDone.wait(5000);

    QCOMPARE(spyDone.count(), 1);
    QCOMPARE(spyErrors.count(), 0);
    QVERIFY(spyFiles.count() > 0 );

}


QTEST_MAIN(ManifestDownloaderTest)

#include "tst_manifestdownloadertest.moc"
