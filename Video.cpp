class Video {
    private:
        int id;
        int views;

    public:
        // Constructor
        Video(int id, int views) : id(id), views(views) {}

        // Getter for id
        int getId() const {
            return id;
        }

        // Getter for views
        int getViews() const {
            return views;
        }

        // Setter for id
        void setId(int videoId) {
            id = videoId;
        }

        // Setter for views
        void setViews(int videoViews) {
            views = videoViews;
        }

    // Overload the << operator to print the Video object
    friend std::ostream& operator<<(std::ostream& os, Video& video) {
        os << "Video ID: " << video.getId() << ", Views: " << video.getViews();
        return os;
    }
};
