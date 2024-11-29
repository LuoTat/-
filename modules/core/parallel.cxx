#include "precomp.hxx"

#include <atomic>

using namespace hl;

namespace hl
{

ParallelLoopBody::~ParallelLoopBody() {}

/* ================================   parallel_for_  ================================ */

static void parallel_for_impl(const hl::Range& range, const hl::ParallelLoopBody& body, double nstripes);    // forward declaration

void parallel_for_(const hl::Range& range, const hl::ParallelLoopBody& body, double nstripes)
{
    if (range.empty())
        return;

    static std::atomic<bool> flagNestedParallelFor(false);
    bool                     isNotNestedRegion = !flagNestedParallelFor.load();
    if (isNotNestedRegion)
        isNotNestedRegion = !flagNestedParallelFor.exchange(true);
    if (isNotNestedRegion)
    {
        try
        {
            parallel_for_impl(range, body, nstripes);
            flagNestedParallelFor = false;
        }
        catch (...)
        {
            flagNestedParallelFor = false;
            throw;
        }
    }
    else    // nested parallel_for_() calls are not parallelized
    {
        HL_UNUSED(nstripes);
        body(range);
    }
}

static void parallel_for_impl(const hl::Range& range, const hl::ParallelLoopBody& body, double nstripes)
{
    HL_UNUSED(nstripes);
    //     using namespace cv::parallel;
    //     if ((numThreads < 0 || numThreads > 1) && range.end - range.start > 1)
    //     {
    //         ParallelLoopBodyWrapperContext ctx(body, range, nstripes);
    //         ProxyLoopBody                  pbody(ctx);
    //         cv::Range                      stripeRange = pbody.stripeRange();
    //         if (stripeRange.end - stripeRange.start == 1)
    //         {
    //             body(range);
    //             return;
    //         }

    //         std::shared_ptr<ParallelForAPI>& api = getCurrentParallelForAPI();
    //         if (api)
    //         {
    //             CV_CheckEQ(stripeRange.start, 0, "");
    //             api->parallel_for(stripeRange.end, parallel_for_cb, (void*)&pbody);
    //             ctx.finalize();    // propagate exceptions if exists
    //             return;
    //         }

    // #ifdef CV_PARALLEL_FRAMEWORK
    //     #if defined HAVE_TBB

    //         #if TBB_INTERFACE_VERSION >= 8000
    //         tbbArena.execute(pbody);
    //         #else
    //         pbody();
    //         #endif

    //     #elif defined HAVE_HPX
    //         pbody();

    //     #elif defined HAVE_OPENMP

    //         #pragma omp parallel for schedule(dynamic) num_threads(numThreads > 0 ? numThreads : numThreadsMax)
    //         for (int i = stripeRange.start; i < stripeRange.end; ++i)
    //             pbody(Range(i, i + 1));

    //     #elif defined HAVE_GCD

    //         dispatch_queue_t concurrent_queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    //         dispatch_apply_f(stripeRange.end - stripeRange.start, concurrent_queue, &pbody, block_function);

    //     #elif defined WINRT

    //         Concurrency::parallel_for(stripeRange.start, stripeRange.end, pbody);

    //     #elif defined HAVE_CONCURRENCY

    //         if (!pplScheduler || pplScheduler->Id() == Concurrency::CurrentScheduler::Id())
    //         {
    //             Concurrency::parallel_for(stripeRange.start, stripeRange.end, pbody);
    //         }
    //         else
    //         {
    //             pplScheduler->Attach();
    //             Concurrency::parallel_for(stripeRange.start, stripeRange.end, pbody);
    //             Concurrency::CurrentScheduler::Detach();
    //         }

    //     #elif defined HAVE_PTHREADS_PF

    //         parallel_for_pthreads(pbody.stripeRange(), pbody, pbody.stripeRange().size());

    //     #else

    //         #error You have hacked and compiling with unsupported parallel framework

    //     #endif

    //         ctx.finalize();    // propagate exceptions if exists
    //         return;
    // #endif                     // CV_PARALLEL_FRAMEWORK
    //     }

    body(range);
}
}    // namespace hl