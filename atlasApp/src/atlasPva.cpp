
#include <epicsVersion.h>
#include <iocsh.h>

#include "fcntl.h"

//#if EPICS_VERSION >= 7


#include <pv/pvAccess.h>
#include <pv/serverContext.h>
#include <pva/server.h>
#include <pva/sharedstate.h>
#include <pv/nt.h>
#include <epicsExport.h>
#include <errlog.h>

namespace pvd = epics::pvData;
namespace pva = epics::pvAccess;

/** Build a CPU info structure **/

static pvd::StructureConstPtr cpuCoreType(
    pvd::getFieldCreate()->createFieldBuilder()
        ->add("vendor", pvd::pvString)
        ->add("model", pvd::pvString)
        ->add("freq", pvd::pvDouble)
        ->add("cache_size", pvd::pvInt)
        ->addArray("features", pvd::pvString)
        ->createStructure()
);

static pvd::StructureConstPtr cpuInfoType(
    pvd::getFieldCreate()->createFieldBuilder()
        ->add("model", pvd::pvString)
        ->addArray("cores", cpuCoreType)
        ->createStructure()
);

class dummyRequester : public pva::ChannelPutRequester
{
public:
    void channelPutConnect(const pvd::Status& status, pva::ChannelPut::shared_pointer const & channelPut, pvd::Structure::const_shared_pointer const & structure) override {}
    void putDone(const pvd::Status& status, pva::ChannelPut::shared_pointer const & channelPut) override {}
    void getDone(const pvd::Status& status, pva::ChannelPut::shared_pointer const & channelPut, pvd::PVStructure::shared_pointer const & pvStructure, pvd::BitSet::shared_pointer const & bitSet) override {}
    std::string getRequesterName() override { return "dummy"; }
};

/**
 * @brief Simple helper to update PV structures
 */
class structUpdater
{
public:
    using shared_ptr = std::shared_ptr<structUpdater>;

    structUpdater() = delete;
    structUpdater(pvd::PVStructure::shared_pointer st) :
        struct_(st),
        bs_(pvd::BitSet::create(st->getNumberFields()))
    {
        for (size_t i = 0; i < st->getNumberFields(); ++i)
            bs_->set(i, true);
    }

    template<typename T>
    void putSubField(const std::string& name, const typename T::value_type& value)
    {
        struct_->getSubField<T>(name)->put(value);
    }

    void putChannel(const pva::Channel::shared_pointer& channel)
    {
        pva::ChannelPutRequester::shared_pointer req(new dummyRequester());
        channel->createChannelPut(req, struct_)->put(struct_, bs_);
    }

protected:
    pvd::PVStructure::shared_pointer struct_;
    pvd::BitSet::shared_pointer bs_;
};

class atlasPvaServer
{
public:
    atlasPvaServer(const char* pvBaseName) :
        provider_("atlasIoc"),
        updater_(new structUpdater(cpuInfoType->build()))
    {
        pv_ = pvas::SharedPV::buildMailbox();
        pv_->open(cpuInfoType);
        provider_.add(std::string(pvBaseName) + ":cpuInfo", pv_);

        server_ = pva::ServerContext::create(
            pva::ServerContext::Config().provider(provider_.provider())
        );

        updater_->putSubField<pvd::PVString>("model", "Test");
        updateChannel_ = provider_.provider()->createChannel(std::string(pvBaseName) + ":cpuInfo");
        updater_->putChannel(updateChannel_);
    }

    void parseCpuInfo()
    {
        FILE* fp = fopen("/proc/cpuinfo", "r");
        if (!fp) {
            errlogPrintf("Unable to open /proc/cpuinfo\n");
            return;
        }

        char* ptr = nullptr;
        size_t l;
        while(getline(&ptr, &l, fp) != -1) {
            if (*ptr == '\n');
        }

        fclose(fp);
    }


    pvas::StaticProvider provider_;
    pvas::SharedPV::shared_pointer pv_;
    pva::ServerContext::shared_pointer server_;
    pva::Channel::shared_pointer updateChannel_;
    structUpdater::shared_ptr updater_;
};

static atlasPvaServer* s_pPvaServer;

// This will expose some structured data from /proc/cpuinfo
void atlasPvaInit(const iocshArgBuf* args)
{
    if (!s_pPvaServer)
        s_pPvaServer = new atlasPvaServer(args[0].sval);
}


//#endif


void atlasPvaRegister()
{
#if EPICS_VERSION >= 7
    static const iocshArg arg0 = {"pvBaseName", iocshArgString};
    static const iocshArg* args[] = {&arg0};
    static const iocshFuncDef func = {"atlasPvaInit", 1, args};
    iocshRegister(&func, atlasPvaInit);
#endif
}

epicsExportRegistrar(atlasPvaRegister);