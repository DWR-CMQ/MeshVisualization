#ifndef EASY3D_UTIL_PROGRESS_H
#define EASY3D_UTIL_PROGRESS_H


#include <string>


namespace MV {

    /**
     * \brief The based class of GUI element reporting the progress.
     * \class ProgressClient MV/util/progress.h
     */
    class ProgressClient {
    public:
        ProgressClient();
        virtual ~ProgressClient() = default;
        virtual void notify(std::size_t percent, bool update_viewer) = 0;
        virtual void cancel();
    };

    //_________________________________________________________

    /**
     * \brief An implementation of progress logging mechanism.
     * \class ProgressLogger MV/util/progress.h
     */
    class ProgressLogger {
    public:
        /// \param max_val The max value (i.e., upper bound) of the progress range.
        /// \param update_viewer \c true to trigger the viewer to update for each step.
        /// \param quiet \c true to make the logger quiet (i.e., don't notify the client).
        ProgressLogger(std::size_t max_val, bool update_viewer, bool quiet = false);
        virtual ~ProgressLogger();

        virtual void notify(std::size_t new_value);
        virtual void next();
        virtual void done() { notify(max_val_); }

        bool is_canceled() const;

        /// Resets the progress logger without changing the progress range.
        void reset() { notify(0); }
        /// Resets the progress logger, and meanwhile changes the progress range.
        void reset(std::size_t max_val);

    protected:
        virtual void update();

    private:
        std::size_t max_val_;
        std::size_t cur_val_;
        std::size_t cur_percent_;
        bool quiet_;
        bool update_viewer_;
    };

    /// A simple progress indicator for console applications. Given percentage = 0.75, the output looks like
    ///     75% [||||||||||||||||||||||||||||||||||||||||||               ]
    void print_progress(float percentage);

}   // namespace MV


#endif  // EASY3D_UTIL_PROGRESS_H

