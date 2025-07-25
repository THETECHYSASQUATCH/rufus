/* vi: set sw=4 ts=4: */
#ifndef UNARCHIVE_H
#define UNARCHIVE_H 1

/* Define busybox macros for cross-platform compatibility */
#ifndef PUSH_AND_SET_FUNCTION_VISIBILITY_TO_HIDDEN
#define PUSH_AND_SET_FUNCTION_VISIBILITY_TO_HIDDEN
#endif

#ifndef USE_FOR_NOMMU
#define USE_FOR_NOMMU(x)
#endif

#ifndef POP_SAVED_FUNCTION_VISIBILITY
#define POP_SAVED_FUNCTION_VISIBILITY
#endif

enum {
#if BB_BIG_ENDIAN
	COMPRESS_MAGIC = 0x1f9d,
	GZIP_MAGIC  = 0x1f8b,
	BZIP2_MAGIC = 256 * 'B' + 'Z',
	/* .xz signature: 0xfd, '7', 'z', 'X', 'Z', 0x00 */
	/* More info at: http://tukaani.org/xz/xz-file-format.txt */
	XZ_MAGIC1   = 256 * 0xfd + '7',
	XZ_MAGIC2   = 256 * (unsigned)(256 * (256 * 'z' + 'X') + 'Z') + 0,
	/* Different form: 32 bits, then 16 bits: */
	/* (unsigned) cast suppresses "integer overflow in expression" warning */
	XZ_MAGIC1a  = 256 * (unsigned)(256 * (256 * 0xfd + '7') + 'z') + 'X',
	XZ_MAGIC2a  = 256 * 'Z' + 0,
	ZSTD_MAGIC1 = 0x28B5,
	ZSTD_MAGIC2 = 0x2FFD,
	ZSTD_MAGIC  = 0x28B52FFD,
#else
	COMPRESS_MAGIC = 0x9d1f,
	GZIP_MAGIC  = 0x8b1f,
	BZIP2_MAGIC = 'B' + 'Z' * 256,
	XZ_MAGIC1   = 0xfd + '7' * 256,
	XZ_MAGIC2   = 'z' + ('X' + ('Z' + 0 * 256) * 256) * 256,
	XZ_MAGIC1a  = 0xfd + ('7' + ('z' + 'X' * 256) * 256) * 256,
	XZ_MAGIC2a  = 'Z' + 0 * 256,
	ZSTD_MAGIC1 = 0xB528,
	ZSTD_MAGIC2 = 0xFD2F,
	ZSTD_MAGIC  = 0xFD2FB528,
#endif
};

typedef struct file_header_t {
	char *name;
	char *link_target;
#if ENABLE_FEATURE_TAR_UNAME_GNAME
	char *tar__uname;
	char *tar__gname;
#endif
	off_t size;
	uid_t uid;
	gid_t gid;
	mode_t mode;
	time_t mtime;
	dev_t device;
} file_header_t;

struct hardlinks_t;

typedef struct archive_handle_t {
	/* Flags. 1st since it is most used member */
	unsigned ah_flags;

	/* The raw stream as read from disk or stdin */
	int src_fd;

	/* Define if the header and data component should be processed */
	char FAST_FUNC (*filter)(struct archive_handle_t *);
	/* List of files that have been accepted */
	llist_t *accept;
	/* List of files that have been rejected */
	llist_t *reject;
	/* List of files that have successfully been worked on */
	llist_t *passed;

	/* Currently processed file's header */
	file_header_t *file_header;

	/* List of link placeholders */
	llist_t *link_placeholders;

	/* Process the header component, e.g. tar -t */
	void FAST_FUNC (*action_header)(const file_header_t *);

	/* Process the data component, e.g. extract to filesystem */
	void FAST_FUNC (*action_data)(struct archive_handle_t *);

	/* Function that skips data */
	void FAST_FUNC (*seek)(int fd, off_t amount);

	/* Count processed bytes */
	off_t offset;

	/* Archiver specific. Can make it a union if it ever gets big */
#if ENABLE_FEATURE_TAR_LONG_OPTIONS
	unsigned tar__strip_components;
#endif
#define PAX_NEXT_FILE 0
#define PAX_GLOBAL    1
#if ENABLE_TAR || ENABLE_DPKG || ENABLE_DPKG_DEB
	smallint tar__end;
# if ENABLE_FEATURE_TAR_GNU_EXTENSIONS
	char* tar__longname;
	char* tar__linkname;
# endif
# if ENABLE_FEATURE_TAR_TO_COMMAND
	char* tar__to_command;
	const char* tar__to_command_shell;
# endif
# if ENABLE_FEATURE_TAR_SELINUX
	char* tar__sctx[2];
# endif
#endif
#if ENABLE_CPIO || ENABLE_RPM2CPIO || ENABLE_RPM
	uoff_t cpio__blocks;
	struct bb_uidgid_t cpio__owner;
	struct hardlinks_t *cpio__hardlinks_to_create;
	struct hardlinks_t *cpio__created_hardlinks;
#endif
#if ENABLE_DPKG || ENABLE_DPKG_DEB
	/* Temporary storage */
	char *dpkg__buffer;
	/* How to process any sub archive, e.g. get_header_tar_gz */
	char FAST_FUNC (*dpkg__action_data_subarchive)(struct archive_handle_t *);
	/* Contains the handle to a sub archive */
	struct archive_handle_t *dpkg__sub_archive;
#endif
#if ENABLE_FEATURE_AR_CREATE
	const char *ar__name;
	struct archive_handle_t *ar__out;
#endif
#if ENABLE_FEATURE_AR_LONG_FILENAMES
	char *ar__long_names;
	unsigned ar__long_name_size;
#endif
} archive_handle_t;
/* bits in ah_flags */
#define ARCHIVE_RESTORE_DATE        (1 << 0)
#define ARCHIVE_CREATE_LEADING_DIRS (1 << 1)
#define ARCHIVE_UNLINK_OLD          (1 << 2)
#define ARCHIVE_EXTRACT_NEWER       (1 << 3)
#define ARCHIVE_DONT_RESTORE_OWNER  (1 << 4)
#define ARCHIVE_DONT_RESTORE_PERM   (1 << 5)
#define ARCHIVE_NUMERIC_OWNER       (1 << 6)
#define ARCHIVE_O_TRUNC             (1 << 7)
#define ARCHIVE_REMEMBER_NAMES      (1 << 8)
#if ENABLE_RPM
#define ARCHIVE_REPLACE_VIA_RENAME  (1 << 9)
#endif


/* POSIX tar Header Block, from POSIX 1003.1-1990  */
#define TAR_BLOCK_SIZE 512
#define NAME_SIZE      100
#define NAME_SIZE_STR "100"
typedef struct tar_header_t {     /* byte offset */
	char name[NAME_SIZE];     /*   0-99 */
	char mode[8];             /* 100-107 */
	char uid[8];              /* 108-115 */
	char gid[8];              /* 116-123 */
	char size[12];            /* 124-135 */
	char mtime[12];           /* 136-147 */
	char chksum[8];           /* 148-155 */
	char typeflag;            /* 156-156 */
	char linkname[NAME_SIZE]; /* 157-256 */
	/* POSIX:   "ustar" NUL "00" */
	/* GNU tar: "ustar  " NUL */
	/* Normally it's defined as magic[6] followed by
	 * version[2], but we put them together to save code.
	 */
	char magic[8];            /* 257-264 */
	char uname[32];           /* 265-296 */
	char gname[32];           /* 297-328 */
	char devmajor[8];         /* 329-336 */
	char devminor[8];         /* 337-344 */
	char prefix[155];         /* 345-499 */
	char padding[12];         /* 500-512 (pad to exactly TAR_BLOCK_SIZE) */
} tar_header_t;
struct BUG_tar_header {
	char c[sizeof(tar_header_t) == TAR_BLOCK_SIZE ? 1 : -1];
};
void chksum_and_xwrite_tar_header(int fd, struct tar_header_t *hp) FAST_FUNC;


extern const char cpio_TRAILER[];


archive_handle_t *init_handle(void) FAST_FUNC;

char filter_accept_all(archive_handle_t *archive_handle) FAST_FUNC;
char filter_accept_list(archive_handle_t *archive_handle) FAST_FUNC;
char filter_accept_list_reassign(archive_handle_t *archive_handle) FAST_FUNC;
char filter_accept_reject_list(archive_handle_t *archive_handle) FAST_FUNC;

void unpack_ar_archive(archive_handle_t *ar_archive) FAST_FUNC;

void data_skip(archive_handle_t *archive_handle) FAST_FUNC;
void data_extract_all(archive_handle_t *archive_handle) FAST_FUNC;
void data_extract_to_stdout(archive_handle_t *archive_handle) FAST_FUNC;
void data_extract_to_command(archive_handle_t *archive_handle) FAST_FUNC;

void header_skip(const file_header_t *file_header) FAST_FUNC;
void header_list(const file_header_t *file_header) FAST_FUNC;
void header_verbose_list(const file_header_t *file_header) FAST_FUNC;

char get_header_ar(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_cpio(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_tar(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_tar_gz(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_tar_bz2(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_tar_lzma(archive_handle_t *archive_handle) FAST_FUNC;
char get_header_tar_xz(archive_handle_t *archive_handle) FAST_FUNC;

void seek_by_jump(int fd, off_t amount) FAST_FUNC;
void seek_by_read(int fd, off_t amount) FAST_FUNC;

const char *strip_unsafe_prefix(const char *str) FAST_FUNC;
void create_or_remember_link(llist_t **link_placeholders,
		const char *target,
		const char *linkname,
		int hard_link) FAST_FUNC;
void create_links_from_list(llist_t *list) FAST_FUNC;

void data_align(archive_handle_t *archive_handle, unsigned boundary) FAST_FUNC;
const llist_t *find_list_entry(const llist_t *list, const char *filename) FAST_FUNC;
const llist_t *find_list_entry2(const llist_t *list, const char *filename) FAST_FUNC;

/* A bit of bunzip2 internals are exposed for compressed help support: */
char *unpack_bz2_data(const char *packed, int packed_len, int unpacked_len) FAST_FUNC;

/* Meaning and direction (input/output) of the fields are transformer-specific */
typedef struct transformer_state_t {
	int8_t   signature_skipped; /* most often referenced member */

	IF_DESKTOP(long long) int FAST_FUNC (*xformer)(struct transformer_state_t *xstate);
	USE_FOR_NOMMU(const char *xformer_prog;)

	/* Source */
	int      src_fd;
	/* Output */
	int      dst_fd;
	const char *dst_dir;            /* if non-NULL, extract to dir */
	char     *dst_name;
	uint64_t dst_size;
	size_t   mem_output_size_max;   /* if non-zero, decompress to RAM instead of fd */
	size_t   mem_output_size;
	char     *mem_output_buf;

	uint64_t bytes_out;
	uint64_t bytes_in;  /* used in unzip code only: needs to know packed size */
	uint32_t crc32;
	time_t   mtime;     /* gunzip code may set this on exit */

	union {             /* if we read magic, it's saved here */
		uint8_t b[8];
		uint16_t b16[4];
		uint32_t b32[2];
	} magic;
} transformer_state_t;

void init_transformer_state(transformer_state_t *xstate) FAST_FUNC;
ssize_t transformer_write(transformer_state_t *xstate, const void *buf, size_t bufsize) FAST_FUNC;
ssize_t xtransformer_write(transformer_state_t *xstate, const void *buf, size_t bufsize) FAST_FUNC;
int check_signature16(transformer_state_t *xstate, unsigned magic16) FAST_FUNC;

static inline int transformer_switch_file(transformer_state_t* xstate)
{
	char dst[MAX_PATH];
	size_t i, last_slash = 0;

	if (xstate->dst_fd > 0) {
#ifdef PLATFORM_WINDOWS
		_close(xstate->dst_fd);
#else
		close(xstate->dst_fd);
#endif
		xstate->dst_fd = -1;
	}
#ifdef PLATFORM_WINDOWS
	_snprintf_s(dst, sizeof(dst), _TRUNCATE, "%s/%s", xstate->dst_dir, xstate->dst_name);
#else
	snprintf(dst, sizeof(dst), "%s/%s", xstate->dst_dir, xstate->dst_name);
#endif
	free(xstate->dst_name);
	xstate->dst_name = NULL;
	for (i = 0; i < strlen(dst); i++) {
		if (dst[i] == '/')
#ifdef PLATFORM_WINDOWS
			dst[i] = '\\';
		if (dst[i] == '\\')
#endif
			last_slash = i;
	}
	if (bled_switch != NULL)
		bled_switch(dst, xstate->dst_size);
	dst[last_slash] = 0;
#ifdef PLATFORM_WINDOWS
	bb_make_directory(dst, 0, 0);
	dst[last_slash] = '/';
	xstate->dst_fd = _openU(dst, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _S_IREAD | _S_IWRITE);
#else
	/* Create directory using mkdir on non-Windows platforms */
	mkdir(dst, 0755);
	dst[last_slash] = '/';
	xstate->dst_fd = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
	if (xstate->dst_fd < 0) {
		bb_error_msg("Could not create '%s' (errno: %d)", dst, errno);
		return -errno;
	}
	return 0;
}

IF_DESKTOP(long long) int inflate_unzip(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_zip_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_Z_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_gz_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_bz2_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_lzma_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_xz_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_vtsi_stream(transformer_state_t *xstate) FAST_FUNC;
IF_DESKTOP(long long) int unpack_zstd_stream(transformer_state_t *xstate) FAST_FUNC;

char* append_ext(char *filename, const char *expected_ext) FAST_FUNC;
int bbunpack(char **argv,
		IF_DESKTOP(long long) int FAST_FUNC (*unpacker)(transformer_state_t *xstate),
		char* FAST_FUNC (*make_new_name)(char *filename, const char *expected_ext),
		const char *expected_ext
) FAST_FUNC;

void check_errors_in_children(int signo);
#if BB_MMU
void fork_transformer(int fd,
	int signature_skipped,
	IF_DESKTOP(long long) int FAST_FUNC (*transformer)(transformer_state_t *xstate)
) FAST_FUNC;
#define fork_transformer_with_sig(fd, transformer, transform_prog) fork_transformer((fd), 0, (transformer))
#define fork_transformer_with_no_sig(fd, transformer)              fork_transformer((fd), 1, (transformer))
#else
void fork_transformer(int fd, const char *transform_prog) FAST_FUNC;
#define fork_transformer_with_sig(fd, transformer, transform_prog) fork_transformer((fd), (transform_prog))
/* fork_transformer_with_no_sig() does not exist on NOMMU */
#endif


POP_SAVED_FUNCTION_VISIBILITY

#endif
